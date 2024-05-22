// Copyright Druid Mechanics


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

// this struct is only for this cpp file so no need for prefix or keyword to expose to other classes
struct AuraDamageStatics
{
	// using macros to easily declare and define AttributeCaptureDef variables, look at MMC classes for long form versions
	// if needing same attribute from both target and source, maybe need to do it long form or make own macro
	// Source Info
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	// Target Info
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	AuraDamageStatics()
	{
		// Source Info
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		// Target Info
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
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
	// Source Info
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	// Target Info
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/* Get Needed Info before Calculations / Boilerplate */
	// Get ASC's
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// Get Avatars
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// Get GE Spec
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// create EvaluateParams needed for AttemptCalculateCapturedAttributeMagnitude()
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	/* End Boilerplate Code */

	/* Get Damage Set by Caller Magnitude */
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	/* Get Levels of Source and Target */
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	int32 SourceLevel = SourceCombatInterface->GetPlayerLevel();
	int32 TargetLevel = TargetCombatInterface->GetPlayerLevel();
	
	/* Getting attribute info */
	/* 1. Set up variable to capture attribute magnitudes
	 * 2. Call function to get attribute info
	 * 3. Clamp any attribute magnitudes as needed
	 */
	/* Source Info */
	// Source Armor Penetration
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	// Source Critical Hit Chance
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Clamp<float>(SourceCriticalHitChance, 0.f, 100.f);

	// Source Critical Hit Damage
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	/* Target Info */
	// Target Armor
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);
	
	// Target Block Chance
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	// Target Critical Hit Resistance
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);
	/* End Attribute Info */

	// Get Coefficient Curve Table Data
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	
	/* Perform the actual calculations needed with previously retrieved info */
	/* 1. 
	 * Check to see if the target blocked the source's attack
	 * If blocked, halve the damage.
	 */
	float RandomNumber = FMath::FRand() * 100;
	bool bAttackBlocked = RandomNumber <= TargetBlockChance ? true : false;
	if (bAttackBlocked)
		Damage = Damage / 2;

	/* 2.
	 * ArmorPenetration ignores a percentage of the Target's Armor.
	 * Armor will reduce a percent of the incoming Damage.
	 * Using coefficients from CT_DamageCalculationCoefficients in the formulas
	 */
	// Armor Pen Coefficient
	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceLevel);

	// Effective Armor Coefficient
	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetLevel);

	// do the actual logic for modifying damage
	const float EffectiveArmor = (TargetArmor * ((100.f - (SourceArmorPenetration * ArmorPenetrationCoefficient)) / 100.f));
	Damage *= ((100 - (EffectiveArmor * EffectiveArmorCoefficient)) / 100.f);

	/* 3.
	 * Check to see if attacked crits
	 * If yes, adjust damage by:
	 *		TotalDMG = BaseDMG * [2 + (CritDMG) - (CritResist)
	 *		CritDMG = Coefficient of Critical Hit Damage
	 *		CritResist = Coefficient of Critical Hit Resist
	 *		TotalDMG >= BaseDMG
	 *
	 *	Did the crit calculation different from tutorial
	 *	If needing to match tutorial, its on #147
	 */
	// Check if attack crits
	RandomNumber = FMath::FRand() * 100;
	bool bAttackCrit = RandomNumber <= SourceCriticalHitChance ? true : false;

	if (bAttackCrit)
	{
		// Critical Hit Damage Coefficient
		FRealCurve* CriticalHitDamageCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitDamage"), FString());
		const float CriticalHitDamageCoefficient = CriticalHitDamageCurve->Eval(SourceLevel);

		// Critical Hit Resistance Coefficient
		FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
		const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetLevel);
		
		// do the actual logic for modifying damage
		float CritMultiplier = 2 + (SourceCriticalHitDamage * CriticalHitDamageCoefficient) - (TargetCriticalHitResistance * CriticalHitResistanceCoefficient);
		CritMultiplier = FMath::Max(1.f, CritMultiplier);
		Damage *= CritMultiplier;
	}
	
	/* Modify Attributes */
	//const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
