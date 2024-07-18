// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FDamageEffectParams;
struct FAbilityDebuffStruct;
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
	
protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category= "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages, bool& bMontageFound) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAbilityDebuffStruct> AbilityDebuffEffects;

	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const;
};
