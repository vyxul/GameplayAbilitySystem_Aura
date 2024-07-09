// Copyright Druid Mechanics


#include "UI/WidgetController/SpellMenuWidgetController.h"

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
	
	/* Binding Abilities Given */
	if (GetAuraASC()->bStartupAbilitiesGiven)
		BroadcastAbilityInfo();
	
	else
		GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	/* PS */
	PlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());

	/* ASC */
	BroadcastAbilityInfo();
}

void USpellMenuWidgetController::SpendPointButtonPressed(const FGameplayTag& AbilityTag)
{
	if (GetAuraASC())
		GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
}

bool USpellMenuWidgetController::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
	FString& OutNextLevelDescription)
{
	FString Description = FString();
	FString NextLevelDescription = FString();
	
	if (GetAuraASC())
	{
		bool bSuccess = GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

		OutDescription = Description;
		OutNextLevelDescription = NextLevelDescription;
		return bSuccess;
	}

	OutDescription = FString();
	OutNextLevelDescription = FString();
	return false;
}
