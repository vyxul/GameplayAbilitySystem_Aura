// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	/*
	// the Damage GE doesn't worry about it's own level
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

	for (auto& Pair : DamageTypes)
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor); 

	if (TargetActor)
		SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
	*/
	
	FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(TargetActor);
	if (DamageEffectParams.TargetASC != nullptr)
		UAuraAbilitySystemLibrary::ApplyAbilityEffect(DamageEffectParams);
}

float UAuraDamageGameplayAbility::GetDamageAtLevel(int32 Level, FGameplayTag DamageTypeTag)
{
	if (DamageTypes.Contains(DamageTypeTag))
		return DamageTypes.Find(DamageTypeTag)->GetValueAtLevel(Level);

	return 0;
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	float AbilityLevel = GetAbilityLevel();
	FDamageEffectParams Params;
	
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageTypes = DamageTypes;
	Params.AbilityStatusEffects = AbilityStatusEffects;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude.GetValueAtLevel(AbilityLevel);
	Params.KnockbackChance = KnockbackInfo.KnockbackChance.GetValueAtLevel(AbilityLevel);
	Params.KnockbackMagnitude = KnockbackInfo.KnockbackMagnitude.GetValueAtLevel(AbilityLevel);

	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f;
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulseDirection = ToTarget;
		Params.KnockbackDirection = ToTarget;
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageOrigin = RadialDamageOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	
	return Params;
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