// Copyright Druid Mechanics


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	/* PS */
	GetAuraPS()->OnPlayerSpellPointsChanged.AddLambda(
		[this](int32 SpellPoints)
		{
			PlayerSpellPointsChanged.Broadcast(SpellPoints);
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