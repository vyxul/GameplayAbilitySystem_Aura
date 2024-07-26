// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Actor/AuraProjectile.h"
#include "Kismet/KismetSystemLibrary.h"


void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileSpawnLocation, const FVector& ProjectileTargetLocation, bool bOverridePitch, float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
		return;

	// AAuraCharacterBase* OwnerActor = Cast<AAuraCharacterBase>(GetAvatarActorFromActorInfo());
	// const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
	// 	OwnerActor,
	// 	FAuraGameplayTags::Get().Combat_Socket_Weapon);
	// Get the rotation to the target
	FRotator Rotation = (ProjectileTargetLocation - ProjectileSpawnLocation).Rotation();
	// Design Decision of Teacher: Make projectile move parallel to the ground
	// if we decide to have gravity on spell, can make pitch higher if we want
	//Rotation.Pitch = 0.f;
	
	if (bOverridePitch)
		Rotation.Pitch = PitchOverride;
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(ProjectileSpawnLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// Get the projectile ready
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// Set up the GE Context to pass along data
	// For now just adding data just for demo purpose, no reason yet
	/*
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	EffectContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);
	
	// Set the GE for the projectile
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	// Set the effect modifier by the GA based off of the GA's damage
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	for (auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Projectile Spell Damage: %f"), ScaledDamage));
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}

	// Set the GE Spec Handle for the Projectile
	Projectile->DamageEffectSpecHandle = SpecHandle;
	*/
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

	// Add faction tag to the projectile
	FName FactionTag = GetAvatarActorFromActorInfo()->ActorHasTag(FName("Player")) ? FName("Player") : FName("Enemy");
	Projectile->Tags.Add(FactionTag);

	// Spawn projectile
	Projectile->FinishSpawning(SpawnTransform);
	
}

void UAuraProjectileSpell::SpawnMultipleProjectiles(const FVector& ProjectileSpawnLocation,
	const FVector& ProjectileTargetLocation, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
		return;

	// using FRotator so that can we adjust pitch if needed,
	// otherwise, could just use normalize functions on FVectors to get directions
	FRotator RotationToTarget = (ProjectileTargetLocation - ProjectileSpawnLocation).Rotation();
	const float TargetDistance = (ProjectileTargetLocation - ProjectileSpawnLocation).Length();

	if (bOverridePitch)
		RotationToTarget.Pitch = PitchOverride;

	const FVector TargetVectorDirection = RotationToTarget.Vector();
	const FVectorSpread FanVectorSpread = GetFanSpread(TargetVectorDirection, ProjectileSpread, NumProjectiles);
	
	float DebugArrowDuration = 5;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	
	// Main Target vector
	UKismetSystemLibrary::DrawDebugArrow(
		AvatarActor,
		ProjectileSpawnLocation,
		ProjectileSpawnLocation + (FanVectorSpread.TargetVectorDirection * TargetDistance),
		5,
		FLinearColor::White,
		DebugArrowDuration,
		2);

	// Edge vectors
	UKismetSystemLibrary::DrawDebugArrow(
		AvatarActor,
		ProjectileSpawnLocation,
		ProjectileSpawnLocation + (FanVectorSpread.LeftEdgeVectorDirection * TargetDistance),
		5,
		FLinearColor::Blue,
		DebugArrowDuration,
		2);
	UKismetSystemLibrary::DrawDebugArrow(
		AvatarActor,
		ProjectileSpawnLocation,
		ProjectileSpawnLocation + (FanVectorSpread.RightEdgeVectorDirection * TargetDistance),
		5,
		FLinearColor::Yellow,
		DebugArrowDuration,
		2);

	// Projectile Vectors
	for (FVector ProjectileVector : FanVectorSpread.VectorDirectionGroup)
	{
		UKismetSystemLibrary::DrawDebugArrow(
			AvatarActor,
			ProjectileSpawnLocation,
			ProjectileSpawnLocation + (ProjectileVector * (TargetDistance / 2)),
			5,
			FLinearColor::Green,
			DebugArrowDuration,
			2
			);
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(ProjectileSpawnLocation);
		SpawnTransform.SetRotation(ProjectileVector.ToOrientationQuat());

		// Get the projectile ready
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// Set Damage Effect Params for projectile
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		// Add faction tag to the projectile
		FName FactionTag = GetAvatarActorFromActorInfo()->ActorHasTag(FName("Player")) ? FName("Player") : FName("Enemy");
		Projectile->Tags.Add(FactionTag);

		if (ProjectileType == EProjectileType::Homing)
		{
			if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
			{
				Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
				Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
				Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
			}

			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
			Projectile->ProjectileMovement->bIsHomingProjectile = true;
		}

		// Spawn projectile
		Projectile->FinishSpawning(SpawnTransform);
	}
}

FVectorSpread UAuraProjectileSpell::GetFanSpread(const FVector& TargetVectorDirection, const float& AngleSpread,
	const int32& ProjectileCount)
{
	// Error case
	if (ProjectileCount <= 0)
		return FVectorSpread();
	
	FVectorSpread FanVectorSpread;
	FanVectorSpread.TargetVectorDirection = TargetVectorDirection;
	FanVectorSpread.AngleSpread = AngleSpread;

	const FVector LeftEdgeVectorDirection = TargetVectorDirection.RotateAngleAxis(-AngleSpread / 2, FVector::UpVector);
	const FVector RightEdgeVectorDirection = TargetVectorDirection.RotateAngleAxis(AngleSpread / 2, FVector::UpVector);
	FanVectorSpread.LeftEdgeVectorDirection = LeftEdgeVectorDirection;
	FanVectorSpread.RightEdgeVectorDirection = RightEdgeVectorDirection;

	FanVectorSpread.ProjectileCount = ProjectileCount;

	// Only 1 projectile, just go straight to target
	if (ProjectileCount == 1)
		FanVectorSpread.VectorDirectionGroup.Add(TargetVectorDirection);

	// Go from left to right for projectiles
	else
	{
		const float DeltaAngle = AngleSpread / (ProjectileCount - 1);
		for (int Projectile = 0; Projectile < ProjectileCount; Projectile++)
		{
			const FVector ProjectileVectorDirection = LeftEdgeVectorDirection.RotateAngleAxis(DeltaAngle * Projectile, FVector::UpVector);
			FanVectorSpread.VectorDirectionGroup.Add(ProjectileVectorDirection);
		}
	}
	
	return FanVectorSpread;
}

FVectorSpread UAuraProjectileSpell::GetShotgunSpread(const FVector& TargetVectorDirection, const float& AngleSpread,
	const int32& ProjectileCount)
{
	return GetFanSpread(TargetVectorDirection, AngleSpread, ProjectileCount);
}
