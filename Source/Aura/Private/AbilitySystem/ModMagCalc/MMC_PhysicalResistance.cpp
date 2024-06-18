// Copyright Druid Mechanics


#include "AbilitySystem/ModMagCalc/MMC_PhysicalResistance.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_PhysicalResistance::UMMC_PhysicalResistance()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Strength, Target, false);

	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_PhysicalResistance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	/*
	 * Formula:
	 *		Physical Resistance = (0.1 * Level) + (0.2 * Strength)
	 */

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// attribute value
	float Strength = 1.f;
	GetCapturedAttributeMagnitude(StrengthDef, Spec, EvaluateParameters, Strength);
	Strength = FMath::Max(1.f, Strength);

	// level value
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	const int32 CharacterLevel = SourceObject->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetPlayerLevel(SourceObject) : 1;

	// get final value
	const float PhysicalResistance = (0.1f * CharacterLevel) + (0.2f * Strength);
	return PhysicalResistance;
}
