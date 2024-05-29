// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_FireResistance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_FireResistance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_FireResistance();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resilience);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Vigor)
};
