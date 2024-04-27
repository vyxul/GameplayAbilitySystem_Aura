// Copyright Druid Mechanics


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// MaxMana = 50 + (2.5 * Intelligence) + (15 * Level)
	
	/* Intelligence Info */
	// Gather tags from source and target, needed for GetCapturedAttributeMagnitude(...)
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Variable to store the magnitude of the captured attribute (IntelligenceDef)
	float Intelligence = 0.f;
	
	// Store magnitude of captured attribute
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);

	// Clamping to ensure no negative number for intelligence
	Intelligence = FMath::Max<float>(Intelligence, 0);

	/* Level Info */
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const float PlayerLevel = CombatInterface->GetPlayerLevel();
	
	/* Calculate the final value of MaxMana */
	const float MaxMana = 50.f + (2.5f * Intelligence) + (15.f * PlayerLevel);
	return MaxMana;
}
