// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/FireBlast.h"

#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"

TArray<AAuraProjectile*> UFireBlast::SpawnFireballs()
{
	TArray<AAuraProjectile*> Fireballs;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, FireballCount);

	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		
		AAuraProjectile* Fireball = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Fireball->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		Fireball->ReturnToActor = GetAvatarActorFromActorInfo();
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		
		FName FactionTag = GetAvatarActorFromActorInfo()->ActorHasTag(FName("Player")) ? FName("Player") : FName("Enemy");
		Fireball->Tags.Add(FactionTag);

		Fireballs.Add(Fireball);
		Fireball->FinishSpawning(SpawnTransform);
	}
	
	return TArray<AAuraProjectile*>();
}

void UFireBlast::ExecuteExplosionGameplayCue() const
{
	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().GameplayCue_FireBlast_Explosion, CueParameters);
}
