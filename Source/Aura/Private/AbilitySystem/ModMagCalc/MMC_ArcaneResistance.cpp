// Copyright Druid Mechanics


#include "AbilitySystem/ModMagCalc/MMC_ArcaneResistance.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_ArcaneResistance::UMMC_ArcaneResistance()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Intelligence, Target, false);

	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_ArcaneResistance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	/* 
	 * Formula:
	 *		Arcane Resist = (0.1 * Level) + (.2 * Intelligence)
	 */

	// create EvaluateParams needed for AttemptCalculateCapturedAttributeMagnitude()
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// get attribute info
	float Intelligence = 1.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);
	Intelligence = FMath::Max(1, Intelligence);

	// get level info
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const float CharacterLevel = CombatInterface->GetPlayerLevel();

	const float ArcaneResistance = (0.1f * CharacterLevel) + (0.2f * Intelligence);
	
	return ArcaneResistance;
}
