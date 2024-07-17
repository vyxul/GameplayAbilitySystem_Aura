// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct FAbilityDebuffStruct
{
	GENERATED_BODY()

	FAbilityDebuffStruct() = default;
	FAbilityDebuffStruct(const FScalableFloat& DebuffChance, const FScalableFloat& DebuffLevel,
	                     const TSubclassOf<UGameplayEffect>& DebuffGameplayEffect)
		: DebuffChance(DebuffChance),
		  DebuffLevel(DebuffLevel),
		  DebuffGameplayEffect(DebuffGameplayEffect)
	{
	}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat DebuffChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat DebuffLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DebuffGameplayEffect;
};

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
};
