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
	void StoreOwnerVariables();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamStartLocation, const FVector& BeamTargetLocation, const float TraceRadius = 10.f);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
	
	UPROPERTY(BlueprintReadWrite, Category= "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
};
