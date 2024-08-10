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
UENUM()
enum EProjectileType
{
	NonHoming = 0 UMETA(DisplayName = "Non-Homing"),
	Homing = 1 UMETA(DisplayName = "Homing")
};

UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	void SpawnProjectile(const FVector& ProjectileSpawnLocation, const FVector& ProjectileTargetLocation, bool bOverridePitch = false, float PitchOverride = 0.f);

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	void SpawnMultipleProjectiles(const FVector& ProjectileSpawnLocation, const FVector& ProjectileTargetLocation, bool bOverridePitch = false, float PitchOverride = 0.f, AActor* HomingTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	void SpawnReturningProjectile();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	int32 NumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	TEnumAsByte<EProjectileType> ProjectileType = EProjectileType::NonHoming;
	
	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category= "Projectile")
	float HomingAccelerationMax = 3200.f;

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	FVectorSpread GetFanSpread(const FVector& TargetVectorDirection, const float& AngleSpread, const int32& ProjectileCount);

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	FVectorSpread GetShotgunSpread(const FVector& TargetVectorDirection, const float& AngleSpread, const int32& ProjectileCount);
};
