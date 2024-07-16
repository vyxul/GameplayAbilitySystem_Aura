// Copyright Druid Mechanics


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	/* PS */
	GetAuraPS()->OnPlayerSpellPointsChanged.AddLambda(
		[this](int32 SpellPoints)
		{
			PlayerSpellPointsChanged.Broadcast(SpellPoints);
		});

	/* ASC */
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
		{
			if (AbilityInfo)
			{
				FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityForTag(AbilityTag);
				AuraAbilityInfo.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(AuraAbilityInfo);
			}
		});
	GetAuraASC()->AbilitySlotInfo.AddUObject(this, &USpellMenuWidgetController::OnAbilitySlotInfoReceived);
	
	/* Binding Abilities Given */
	if (GetAuraASC()->bStartupAbilitiesGiven)
		BroadcastAbilityInfo();
	
	else
		GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &USpellMenuWidgetController::BroadcastAbilityInfo);
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	/* PS */
	PlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());

	/* ASC */
	BroadcastAbilityInfo();
}

FAuraAbilityInfo USpellMenuWidgetController::GetAbilityInfoByTag(const FGameplayTag& AbilityTag)
{
	if (AbilityInfo)
		return AbilityInfo->FindAbilityForTag(AbilityTag);

	return FAuraAbilityInfo();
}

void USpellMenuWidgetController::SpendPointButtonPressed(const FGameplayTag& AbilityTag)
{
	if (GetAuraASC())
		GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
}

void USpellMenuWidgetController::EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag, const FGameplayTag& AbilityType)
{
	FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityForTag(AbilityTag);
	if (AuraAbilityInfo.AbilityType.MatchesTagExact(AbilityType))
		GetAuraASC()->ServerEquipAbility(AbilityTag, InputTag);
}

bool USpellMenuWidgetController::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                             FString& OutNextLevelDescription)
{
	FString Description = FString();
	FString NextLevelDescription = FString();

	const FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityForTag(AbilityTag);
	// If Ability not in AbilityInfo
	if (!AuraAbilityInfo.AbilityTag.IsValid())
	{
		OutDescription = FString("AbilityTag not found in AbilityInfo");
		return false;
	}

	// If AuraASC not assigned yet
	if (!GetAuraASC())
	{
		OutDescription = FString();
		OutNextLevelDescription = FString();
		return false;
	}

	const bool bSuccess = GetAuraASC()->GetDescriptionsByAbilityTag(AuraAbilityInfo, Description, NextLevelDescription);
	OutDescription = Description;
	OutNextLevelDescription = NextLevelDescription;
	return bSuccess;
}