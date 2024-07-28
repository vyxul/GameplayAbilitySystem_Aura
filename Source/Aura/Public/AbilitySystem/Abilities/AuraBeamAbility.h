// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamAbility : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerPlayerController();
	
protected:
	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
};
