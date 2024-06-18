// Copyright Druid Mechanics


#include "AbilitySystem/ModMagCalc/MMC_FireResistance.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_FireResistance::UMMC_FireResistance()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resilience, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Vigor, Target, false);

	RelevantAttributesToCapture.Add(ResilienceDef);
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_FireResistance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	/*
	 * Formula:
	 *		Fire Resistance = (0.1 * Level) + (0.12 * Resilience) + (0.08 * Vigor)
	 */

	// used to get attribute magnitude
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// attribute info
	float Resilience = 1.f;
	GetCapturedAttributeMagnitude(ResilienceDef, Spec, EvaluateParameters, Resilience);
	Resilience = FMath::Max(1.f, Resilience);

	float Vigor = 1.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max(1.f, Vigor);

	// character level
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const float CharacterLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());;
	
	const float FireResistance = (0.1f * CharacterLevel) + (0.12f * Resilience) + (0.08f * Vigor);
	return FireResistance;
}
