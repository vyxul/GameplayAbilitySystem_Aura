// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class UAbilityInfo;
/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	FGameplayTag StartupInputTag;

	FStringFormatNamedArguments GetAbilityDescriptionFormatter(UGameplayAbility* GameplayAbility, int32 Level);
	
	UFUNCTION(BlueprintCallable)
	FString GetFormattedDescription(UGameplayAbility* GameplayAbility, int32 Level, FString LiteralDescription);
	
	UFUNCTION(BlueprintNativeEvent)
	FString GetDescription(UAbilityInfo* AbilityInfo, int32 Level);
	
	static FString GetLockedDescription(int32 Level);

protected:
	float GetManaCost(float InLevel = 1.f);
	float GetCooldown(float InLevel = 1.f);
};
