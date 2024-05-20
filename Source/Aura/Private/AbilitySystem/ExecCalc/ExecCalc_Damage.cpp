// Copyright Druid Mechanics


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

// this struct is only for this cpp file so no need for prefix or keyword to expose to other classes
struct AuraDamageStatics
{
	// using macros to easily declare and define AttributeCaptureDef variables, look at MMC classes for long form versions
	// if needing same attribute from both target and source, maybe need to do it long form or make own macro
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/* Get Needed Info before Calculations / Boilerplate */
	// Get ASC's
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// Get Avatars
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// Get GE Spec
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// create EvaluateParams needed for AttemptCalculateCapturedAttributeMagnitude()
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	/* End Boilerplate Code */

	/* Get Damage Set by Caller Magnitude */
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
	/* Getting attribute info */
	// set up variables to capture attribute magnitudes
	//float Armor = 0.f;
	// Capture BlockChance on Target, and determine if there was a successful block
	// If Block, halve the damage.
	float BlockChance = 0.f;
	
	// Call function to get attribute info
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, Armor);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, BlockChance);

	// clamp any attribute magnitudes as needed
	// this is to clamp armor to be > 0, doing simple modifier for now for demo
	//Armor = FMath::Max<float>(0.f, Armor);
	//++Armor;
	BlockChance = FMath::Max<float>(0.f, BlockChance);

	// Perform any calculations needed with the attribute magnitudes retrieved
	float RandomNumber = FMath::FRand() * 100;
	bool bAttackBlocked = RandomNumber <= BlockChance ? true : false;
	if (bAttackBlocked)
		Damage = Damage / 2;

	/* Modify Attributes */
	//const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
