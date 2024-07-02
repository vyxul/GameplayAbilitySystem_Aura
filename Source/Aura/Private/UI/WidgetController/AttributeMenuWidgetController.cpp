// Copyright Druid Mechanics


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo);
	
	for (auto& Tag : AttributeInfo.Get()->AttributeInformation)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Tag.AttributeGetter)
        	.AddLambda(
        		[this, Tag](const FOnAttributeChangeData& Data)
        		{
        			BroadcastAttributeInfo(Tag.AttributeTag);
        		});
	}

	/* Bind PS */
	// Attribute Points
	GetAuraPS()->OnPlayerAttributePointsChanged.AddLambda(
		[this](int32 AttributePoints)
		{
			OnPlayerAttributePointsChanged.Broadcast(AttributePoints);
		});
	// Spell Points
	GetAuraPS()->OnPlayerSpellPointsChanged.AddLambda(
		[this](int32 SpellPoints)
		{
			OnPlayerSpellPointsChanged.Broadcast(SpellPoints);
		});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);

	for (auto& Tag : AttributeInfo.Get()->AttributeInformation)
	{
		BroadcastAttributeInfo(Tag.AttributeTag);
	}

	/* PS */
	OnPlayerAttributePointsChanged.Broadcast(GetAuraPS()->GetAttributePoints());
	OnPlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
