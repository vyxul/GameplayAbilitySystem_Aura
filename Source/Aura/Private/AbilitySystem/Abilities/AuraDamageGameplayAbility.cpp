// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// the Damage GE doesn't worry about it's own level
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

	for (auto& Pair : DamageTypes)
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetActor)
		SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}

float UAuraDamageGameplayAbility::GetDamageAtLevel(int32 Level, FGameplayTag DamageTypeTag)
{
	if (DamageTypes.Contains(DamageTypeTag))
		return DamageTypes.Find(DamageTypeTag)->GetValueAtLevel(Level);

	return 0;
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages, bool& bMontageFound) const
{
	if (TaggedMontages.Num() > 0)
	{
		bMontageFound = true;
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}

	bMontageFound = false;
	return FTaggedMontage();
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageTypes = DamageTypes;
	Params.AbilityDebuffEffects = AbilityDebuffEffects;
	
	return Params;
}
