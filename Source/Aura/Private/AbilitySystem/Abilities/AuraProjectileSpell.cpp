// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Aura/Public/AuraGameplayTags.h"
#include "Character/AuraCharacterBase.h"


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

	// Add faction tag to the projectile
	FName FactionTag = GetAvatarActorFromActorInfo()->ActorHasTag(FName("Player")) ? FName("Player") : FName("Enemy");
	Projectile->Tags.Add(FactionTag);

	// Spawn projectile
	Projectile->FinishSpawning(SpawnTransform);
	
}
