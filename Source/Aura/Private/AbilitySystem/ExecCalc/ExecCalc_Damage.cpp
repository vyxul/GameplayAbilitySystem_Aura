// Copyright Druid Mechanics


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

// this struct is only for this cpp file so no need for prefix or keyword to expose to other classes
struct AuraDamageStatics
{
	// using macros to easily declare and define AttributeCaptureDef variables, look at MMC classes for long form versions
	// if needing same attribute from both target and source, maybe need to do it long form or make own macro
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
	}
};

// static function with static property makes it so we will always get the same AuraDamageStatics struct
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/* Get Needed Info before Calculations */
	// Get ASC's
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// Get Avatars
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// Get GE Spec
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	/* Getting attribute info */
	// create EvaluateParams needed for AttemptCalculateCapturedAttributeMagnitude()
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// set up variables to capture attribute magnitudes
	float Armor = 0.f;
	
	// Call function to get attribute info
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, Armor);

	// clamp any attribute magnitudes as needed
	// this is to clamp armor to be > 0, doing simple modifier for now for demo
	Armor = FMath::Max<float>(0.f, Armor);
	++Armor;

	/* Modify Attributes */
	const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
