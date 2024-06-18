// Copyright Druid Mechanics


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
}

// Used to set up property replication
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Need to explicitly setup each property that needs to be replicated
	/* Vital Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	
	/* Primary Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/* Secondary Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	/* Resistances */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
}

/*
 * Function called before attributes are changed, recommended to be only for clamping and not logic.
 * Does not fully change the value, look at PostGameplayEffectExecute
 * The NewValue is pretty much just the result from a query
 */
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AttributeSet)

	/* Effect Context */
	EffectProperties.EffectContextHandle = Data.EffectSpec.GetContext();

	/* Source Information */
	EffectProperties.SourceASC = EffectProperties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(EffectProperties.SourceASC) &&
		EffectProperties.SourceASC->AbilityActorInfo.IsValid() &&
		EffectProperties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.SourceAvatarActor = EffectProperties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		EffectProperties.SourceController = EffectProperties.SourceASC->AbilityActorInfo->PlayerController.Get();

		// If SourceController isn't valid from SourceASC->AbilityActorInfo, set it up from the Source Pawn if available
		if (EffectProperties.SourceController == nullptr &&
			EffectProperties.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(EffectProperties.SourceAvatarActor))
			{
				EffectProperties.SourceController = Pawn->GetController();
			}
		}
		if (EffectProperties.SourceController)
		{
			EffectProperties.SourceCharacter = Cast<ACharacter>(EffectProperties.SourceController->GetPawn());
		}
	}

	/* Target Information */
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		EffectProperties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		EffectProperties.TargetCharacter = Cast<ACharacter>(EffectProperties.TargetAvatarActor);
		EffectProperties.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(EffectProperties.TargetAvatarActor);
	}
}

/**
 * Function called after Gameplay Effects are executed.
 * Clamping attributes here will fully take effect
 */
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("UAuraAttributeSet::PostGameplayEffectExecute(): Changed Health on %s, Health: %f"), *EffectProperties.TargetAvatarActor->GetName(), GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalIncomingDamage > 0)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0, GetMaxHealth()));
			UE_LOG(LogAura, Display, TEXT("UAuraAttributeSet::PostGameplayEffectExecute(): Changed Health on %s, Health: %f"), *EffectProperties.TargetAvatarActor->GetName(), GetHealth());

			const bool bFatal = NewHealth <= 0;

			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
				SendXPEvent(EffectProperties);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				EffectProperties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			ShowFloatingText(EffectProperties, LocalIncomingDamage);
		}
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalIncomingXP = GetIncomingXP();
		SetIncomingXP(0.f);
		
		UE_LOG(LogAura, Log, TEXT("Incoming XP: %f"), LocalIncomingXP)
		
		// Source Character is the owner, since GA_ListenForEvents applies GE_EventBasedEffect, adding to IncomingXP
		ACharacter* SourceCharacter = EffectProperties.SourceCharacter;
		if (SourceCharacter->Implements<UPlayerInterface>() && SourceCharacter->Implements<UCombatInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(SourceCharacter);
			const int32 CurrentXP = IPlayerInterface::Execute_GetXP(SourceCharacter);

			UDataTable* LevelUpInfo = IPlayerInterface::Execute_GetLevelUpInfo(SourceCharacter);
			const int32 LevelAfterAddingXP = ULevelUpInfo::FindLevelForXP(LevelUpInfo, (CurrentXP + LocalIncomingXP));

			const int32 NumLevelUps = LevelAfterAddingXP - CurrentLevel;
			
			if (NumLevelUps >= 1)
			{
				// 1. Add to Player Level
				IPlayerInterface::Execute_AddToPlayerLevel(SourceCharacter, NumLevelUps);
				
				// 2. Add to Attribute and Spell Point
				int32 AttributePointsReward = 0;
				int32 SpellPointsReward = 0;

				for (int LevelPassed = CurrentLevel; LevelPassed < LevelAfterAddingXP; LevelPassed++)
				{
					int32 AttributePointsGainedThisLevel = IPlayerInterface::Execute_GetAttributePointsReward(SourceCharacter, LevelPassed);
					int32 SpellPointsGainedThisLevel = IPlayerInterface::Execute_GetSpellPointsReward(SourceCharacter, LevelPassed);

					AttributePointsReward += AttributePointsGainedThisLevel;
					SpellPointsReward += SpellPointsGainedThisLevel;
				}

				IPlayerInterface::Execute_AddToAttributePoints(SourceCharacter, AttributePointsReward);
				IPlayerInterface::Execute_AddToSpellPoints(SourceCharacter, SpellPointsReward);

				// 3. Fill up Health and Mana
				SetHealth(GetMaxHealth());
				SetMana(GetMaxMana());
				
				// 4. Visual FX
				IPlayerInterface::Execute_LevelUp(SourceCharacter);
			}
			
			IPlayerInterface::Execute_AddToXP(SourceCharacter, LocalIncomingXP);
		}
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& EffectProperties, float Damage) const
{
	// Show damage number as long as it's not self damage
	if (EffectProperties.SourceCharacter != EffectProperties.TargetCharacter)
	{
		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(EffectProperties.EffectContextHandle);
		const bool bCriticalHit  = UAuraAbilitySystemLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);

		// for when player deals damage
		AAuraPlayerController* PC = Cast<AAuraPlayerController>(EffectProperties.SourceCharacter->Controller);
		if (PC)
		{
			PC->ShowDamageNumber(EffectProperties.TargetCharacter, Damage, bBlockedHit, bCriticalHit);
			return;
		}
		
		// for when player receives damage
		PC = Cast<AAuraPlayerController>(EffectProperties.TargetCharacter->Controller);
		if (PC)
			PC->ShowDamageNumber(EffectProperties.TargetCharacter, Damage, bBlockedHit, bCriticalHit);
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& EffectProperties)
{
	AActor* TargetCharacter = EffectProperties.TargetCharacter;
	if (TargetCharacter->Implements<UCombatInterface>())
	{
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(TargetCharacter);
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(TargetCharacter);
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPForClassAndLevel(TargetCharacter, TargetClass, TargetLevel);

		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		
		FGameplayEventData Payload = FGameplayEventData();
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EffectProperties.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}

/* Attribute OnRep_Notify Functions */
/* Vital Attributes */
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

/* Primary Attributes */
void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

/* Secondary Attributes */
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldArcaneResistance);
}
