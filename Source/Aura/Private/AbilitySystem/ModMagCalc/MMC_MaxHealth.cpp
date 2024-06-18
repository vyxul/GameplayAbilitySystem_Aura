// Copyright Druid Mechanics


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// MaxHealth = 80 + (2.5 * Vigor) + (10 * Level)

	/* Vigor Info */
	// Gather tags from source and target, needed for GetCapturedAttributeMagnitude(...)
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Variable to store the magnitude of the capture attribute (VigorDef)
	float Vigor = 0.f;

	// Store magnitude of captured attribute
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);

	// Clamping to ensure no negative number for Vigor
	Vigor = FMath::Max<float>(Vigor, 0);

	
	/* Level Info */
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());

	/* Calculate the final value of MaxHealth */
	const float MaxHealth = 80.f + (2.5f * Vigor) + (10.f * PlayerLevel);
	return MaxHealth;
}
