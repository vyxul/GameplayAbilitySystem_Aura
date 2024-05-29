// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_ArcaneResistance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_ArcaneResistance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_ArcaneResistance();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence);
};
