// Copyright Druid Mechanics


#include "UI/WidgetController/AttributeMenuWidgetController.h"

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
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	// Attribute Points
	AuraPS->OnPlayerAttributePointsChanged.AddLambda(
		[this](int32 AttributePoints)
		{
			OnPlayerAttributePointsChanged.Broadcast(AttributePoints);
		});
	// Spell Points
	AuraPS->OnPlayerSpellPointsChanged.AddLambda(
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
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	OnPlayerAttributePointsChanged.Broadcast(AuraPS->GetAttributePoints());
	OnPlayerSpellPointsChanged.Broadcast(AuraPS->GetSpellPoints());
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
