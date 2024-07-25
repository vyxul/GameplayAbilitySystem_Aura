// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	void SpawnProjectile(const FVector& ProjectileSpawnLocation, const FVector& ProjectileTargetLocation, bool bOverridePitch = false, float PitchOverride = 0.f);

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	void SpawnMultipleProjectiles(const FVector& ProjectileSpawnLocation, const FVector& ProjectileTargetLocation, bool bOverridePitch = false, float PitchOverride = 0.f, bool bHomingProjectiles = false, AActor* HomingTarget = nullptr);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	int32 NumProjectiles = 5;

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	FVectorSpread GetFanSpread(const FVector& TargetVectorDirection, const float& AngleSpread, const int32& ProjectileCount);

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	FVectorSpread GetShotgunSpread(const FVector& TargetVectorDirection, const float& AngleSpread, const int32& ProjectileCount);
};
