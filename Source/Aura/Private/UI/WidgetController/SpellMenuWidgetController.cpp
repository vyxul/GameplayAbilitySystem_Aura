// Copyright Druid Mechanics


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	/* PS */
	GetAuraPS()->OnPlayerSpellPointsChanged.AddLambda(
		[this](int32 SpellPoints)
		{
			PlayerSpellPointsChanged.Broadcast(SpellPoints);
		});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	/* PS */
	PlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}