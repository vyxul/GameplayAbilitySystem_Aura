// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "FireBlast.generated.h"

class AAuraProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category= "Projectile")
	TArray<AAuraProjectile*> SpawnFireballs();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AAuraProjectile*> Projectiles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 FireballCount = 1;
};
