// Copyright Druid Mechanics


#include "AbilitySystem/ModMagCalc/MMC_LightningResistance.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_LightningResistance::UMMC_LightningResistance()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resilience, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Vigor, Target, false);

	RelevantAttributesToCapture.Add(ResilienceDef);
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_LightningResistance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	/*
	 * Formula:
	 *		Lightning Resistance = (0.1 * Level) + (0.08 * Resilience) + (0.12 * Vigor)
	 */

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// attribute values
	float Resilience = 1.f;
	GetCapturedAttributeMagnitude(ResilienceDef, Spec, EvaluateParameters, Resilience);
	Resilience = FMath::Max(1.f, Resilience);

	float Vigor = 1.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max(1.f, Vigor);

	// level value
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	const int32 CharacterLevel = SourceObject->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetPlayerLevel(SourceObject) : 1;

	// get final value
	const float LightningResistance = (0.1f * CharacterLevel) + (0.08f * Resilience) + (0.12f * Vigor);
	return LightningResistance;
}
