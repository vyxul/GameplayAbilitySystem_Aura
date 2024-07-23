// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FAbilityStatusEffectStruct;
struct FDamageEffectParams;
/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel(int32 Level, FGameplayTag DamageTypeTag);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
	
protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category= "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UPROPERTY(EditDefaultsOnly, Category= "Damage")
	FScalableFloat DeathImpulseMagnitude;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages, bool& bMontageFound) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAbilityStatusEffectStruct> AbilityStatusEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FKnockbackStruct KnockbackInfo;
};
