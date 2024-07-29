// Copyright Druid Mechanics


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "PropertyPathHelpers.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWidgetControllerParams.PlayerController = PC;
			OutWidgetControllerParams.PlayerState = PS;
			OutWidgetControllerParams.AbilitySystemComponent = ASC;
			OutWidgetControllerParams.AttributeSet = AS;

			OutAuraHUD = AuraHUD;

			return true;
		}
	}

	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
		return AuraHUD->GetOverlayWidgetController(WCParams);

	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);

	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
		return AuraHUD->GetSpellMenuWidgetController(WCParams);

	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	// Get Actor reference to set as source later for GE Context Handle source
	AActor* AvatarActor = ASC->GetAvatarActor();

	// Get Class Info
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo == nullptr)
		return;
	
	FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

	/* Apply Attributes to ASC */
	// Get EffectContextHandle Ready
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(AvatarActor);
	
	// Apply Primary Attributes
	FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, EffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	// Apply Secondary Attributes
	FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, EffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
	
	// Apply Vital Attributes
	FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, EffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	// Get Class Info
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo == nullptr)
		return;

	// Give the common abilities
	for (TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	// Give and Activate the passive common abilities
	for (TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbilityAndActivateOnce(AbilitySpec);
	}

	// Give the class specific abilities
	for (const auto AbilityClass : ClassInfo->GetClassDefaultInfo(CharacterClass).ClassAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			int32 CharacterLevel = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CharacterLevel);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetXPForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
		return 0;

	FCharacterClassDefaultInfo* ClassDefaultInfo = CharacterClassInfo->CharacterClassInformation.Find(CharacterClass);
	if (ClassDefaultInfo == nullptr)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find class default info for class [%s]"), *UEnum::GetValueAsString(CharacterClass))
		return 0;
	}

	return static_cast<int32>(ClassDefaultInfo->XPReward.GetValueAtLevel(Level));
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	// Get Gamemode reference, which has reference to the Character Class Info
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)
		return nullptr;

	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	// Get Gamemode reference, which has reference to the Ability Info
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)
		return nullptr;

	return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		return AuraEffectContext->IsBlockedHit();

	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		return AuraEffectContext->IsCriticalHit();
	
	return false;
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		return AuraEffectContext->GetDeathImpulse();

	return FVector::Zero();
}

FVector UAuraAbilitySystemLibrary::GetKnockback(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		return AuraEffectContext->GetKnockback();

	return FVector::Zero();
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,	bool bInIsCriticalHit)
{
	FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InDeathImpulse)
{
	FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		AuraEffectContext->SetDeathImpulse(InDeathImpulse);
}

void UAuraAbilitySystemLibrary::SetKnockback(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InKnockback)
{
	FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext)
		AuraEffectContext->SetKnockback(InKnockback);
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                           TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                           const FVector& SphereOrigin)
{
	// Set up collision params
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// Set up the array holding results of query
	TArray<FOverlapResult> Overlaps;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (World)
	{
		World->OverlapMultiByObjectType(
			Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams
		);

		for (FOverlapResult& Overlap : Overlaps)
		{
			const bool ImplementsCombatInterface = Overlap.GetActor()->Implements<UCombatInterface>();
			if (ImplementsCombatInterface)
			{
				const bool IsAlive = !ICombatInterface::Execute_IsDead(Overlap.GetActor());
				if (IsAlive)
					OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Targets,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Targets.Num() <= MaxTargets)
	{
		OutClosestTargets = Targets;
		return;
	}

	TArray<AActor*> TargetsToCheck = Targets;
	int32 NumTargetsFound = 0;

	/* Basic way O(N^2) */
	while (NumTargetsFound < MaxTargets)
	{
		AActor* ClosestActor;
		double ClosestDistance = TNumericLimits<double>::Max();
		
		for (AActor* PotentialTarget : TargetsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestActor = PotentialTarget;
				ClosestDistance = Distance;
			}
		}

		OutClosestTargets.AddUnique(ClosestActor);
		TargetsToCheck.Remove(ClosestActor);
		NumTargetsFound++;
	}

	/* Using built-in sort function, likely to be a O(N*logN) algorithm */
	/*
	TargetsToCheck.Sort(
		[Origin](AActor& A, AActor& B)
		{
			float DistanceAToOrigin = FVector::Distance(A.GetActorLocation(), Origin);
			float DistanceBToOrigin = FVector::Distance(B.GetActorLocation(), Origin);
			return DistanceAToOrigin < DistanceBToOrigin;
		});

	for (AActor* PotentialTarget : TargetsToCheck)
	{
		OutClosestTargets.Add(PotentialTarget);
	}
	*/
}

bool UAuraAbilitySystemLibrary::AreOpposingFactions(AActor* FirstActor, AActor* SecondActor)
{
	const bool FirstIsPlayer = FirstActor->ActorHasTag(FName("Player"));
	const bool SecondIsPlayer = SecondActor->ActorHasTag(FName("Player"));

	return (FirstIsPlayer != SecondIsPlayer);
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyAbilityEffect(FDamageEffectParams DamageEffectParams)
{
	AActor* SourceAvatar = DamageEffectParams.SourceASC->GetAvatarActor();
	AActor* TargetAvatar = DamageEffectParams.TargetASC->GetAvatarActor();

	// Damage Effect
	FGameplayEffectContextHandle DamageContextHandle = DamageEffectParams.SourceASC->MakeEffectContext();
	
	DamageContextHandle.AddSourceObject(SourceAvatar);
	const FVector DeathImpulse = DamageEffectParams.DeathImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	SetDeathImpulse(DamageContextHandle, DeathImpulse);

	float KnockbackRandomFloat = FMath::FRandRange(0.f, 100.f);
	if (KnockbackRandomFloat <= DamageEffectParams.KnockbackChance)
	{
		const FVector Knockback = DamageEffectParams.KnockbackDirection * DamageEffectParams.KnockbackMagnitude;
		SetKnockback(DamageContextHandle, Knockback);
	}
	
	FGameplayEffectSpecHandle DamageSpecHandle =
		DamageEffectParams.SourceASC->MakeOutgoingSpec(
			DamageEffectParams.DamageGameplayEffectClass,
			DamageEffectParams.AbilityLevel,
			DamageContextHandle);
	
	for (auto& Pair : DamageEffectParams.DamageTypes)
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			DamageSpecHandle,
			Pair.Key,
			Pair.Value.GetValueAtLevel(DamageEffectParams.AbilityLevel));
	
	DamageEffectParams.SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data, DamageEffectParams.TargetASC);
	
	// Status Effects
	float RandomFloat = FMath::FRandRange(0.f, 100.f);
	for (FAbilityStatusEffectStruct& StatusEffect : DamageEffectParams.AbilityStatusEffects)
	{
		if (StatusEffect.StatusEffectTag == FGameplayTag::EmptyTag ||
			StatusEffect.StatusEffectChance == FScalableFloat() ||
			StatusEffect.StatusEffectLevel == FScalableFloat())
			continue;
		
		if (RandomFloat <= StatusEffect.StatusEffectChance.GetValueAtLevel(DamageEffectParams.AbilityLevel))
		{
			float StatusEffectLevel = StatusEffect.StatusEffectLevel.GetValueAtLevel(DamageEffectParams.AbilityLevel);
			
			// send a gameplay event tag like UAuraAttributeSet::SendXPEvent
			// have GA_ListenForStatusEffects handle applying the status effect
			// that GA will have needed info for the status effect
			// pass in SourceAvatar, StatusEffectlevel, and StatusEffectTag with the payload
			// not sure if need to get some reference to the status effect, worry about it later
			// if need it, can make this function return array of EffectContextHandles and store them maybe
			
			FGameplayEventData Payload = FGameplayEventData();
			Payload.EventTag = StatusEffect.StatusEffectTag;
			Payload.Instigator = SourceAvatar;
			Payload.Target = TargetAvatar;
			Payload.EventMagnitude = StatusEffectLevel;
			// dont know if need this
			Payload.ContextHandle = DamageContextHandle;
			Payload.OptionalObject = DamageEffectParams.SourceASC;
			Payload.OptionalObject2 = DamageEffectParams.TargetASC;

			/*
			 * Encountering problems when having GA_ListenForStatusEffects being called on the target to apply to itself, causes
			 * instigator and effect causer to be target instead of the actual instigator / effect causer
			 * Try having that ability on all still but it applies the gameplay spec on target instead
			 */
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceAvatar, StatusEffect.StatusEffectTag, Payload);
		}
	}
	
	return DamageContextHandle;
}

float UAuraAbilitySystemLibrary::GetScalableFloatValueAtLevel(FScalableFloat ScalableFloat, float Level)
{
	return ScalableFloat.GetValueAtLevel(Level);
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;

	const FVector LeftEdge = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftEdge.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlySpacedVectors(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;

	const FVector LeftEdge = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftEdge.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}
