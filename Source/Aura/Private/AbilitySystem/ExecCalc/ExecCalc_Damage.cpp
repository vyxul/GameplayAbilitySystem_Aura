// Copyright Druid Mechanics


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
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
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> GetTagsToCaptureDef()
	{
		if (TagsToCaptureDefs.IsEmpty())
			InitMap();
		
		return TagsToCaptureDefs;
	}
	
	AuraDamageStatics()
	{
		/* Definitions */
		// Source Info
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		// Target Info
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}

private:
	void InitMap()
	{
		/* Map Tags To Capture Defs */
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		// Source
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		// Target
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	};
};
	
// static function with static property makes it so we will always get the same AuraDamageStatics struct
static AuraDamageStatics& GetDamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Source Info
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitDamageDef);
	// Target Info
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().PhysicalResistanceDef);
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

	// Get GE Context Handle
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
	// create EvaluateParams needed for AttemptCalculateCapturedAttributeMagnitude()
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	/* End Boilerplate Code */

	/* Get Damage Set by Caller Magnitude */
	float Damage = 0.f;
	for (const auto& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag DamageTypeResistanceTag = Pair.Value;
		
		checkf(GetDamageStatics().GetTagsToCaptureDef().Contains(DamageTypeResistanceTag),
			TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"),
			*DamageTypeResistanceTag.ToString());
		
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = GetDamageStatics().GetTagsToCaptureDef()[DamageTypeResistanceTag];

		//TODO: Can decide to either skip current loop if the EffectSpec not using the current DamageTypeTag or to not warn console 
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue *= (100.f - Resistance) / 100.f;
		Damage += DamageTypeValue;
	}

	/* Get Levels of Source and Target */
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	int32 SourceLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	
	/* Getting attribute info */
	/* 1. Set up variable to capture attribute magnitudes
	 * 2. Call function to get attribute info
	 * 3. Clamp any attribute magnitudes as needed
	 */
	/* Source Info */
	// Source Armor Penetration
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	// Source Critical Hit Chance
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Clamp<float>(SourceCriticalHitChance, 0.f, 100.f);

	// Source Critical Hit Damage
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	/* Target Info */
	// Target Armor
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);
	
	// Target Block Chance
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	// Target Critical Hit Resistance
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);

	// Target Arcane Resistance
	float TargetArcaneResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetArcaneResistance);
	TargetArcaneResistance = FMath::Max<float>(0.f, TargetArcaneResistance);

	// Target Fire Resistance
	float TargetFireResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetFireResistance);
	TargetFireResistance = FMath::Max<float>(0.f, TargetFireResistance);

	// Target Lightning Resistance
	float TargetLightningResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetLightningResistance);
	TargetLightningResistance = FMath::Max<float>(0.f, TargetLightningResistance);

	// Target Physical Resistance
	float TargetPhysicalResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetPhysicalResistance);
	TargetPhysicalResistance = FMath::Max<float>(0.f, TargetPhysicalResistance);
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
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bAttackBlocked);

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
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bAttackCrit);
	
	/* Modify Attributes */
	//const FGameplayModifierEvaluatedData EvaluatedData(GetDamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
