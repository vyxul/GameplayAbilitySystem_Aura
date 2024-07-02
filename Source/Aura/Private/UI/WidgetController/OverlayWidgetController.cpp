// Copyright Druid Mechanics


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

/* Macro to easily bind attributes to a lambda function */
#define BIND_CALLBACK(AttributeName) AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->Get##AttributeName##Attribute())\
	.AddLambda( \
		[this](const FOnAttributeChangeData& Data) \
		{ \
			On##AttributeName##Changed.Broadcast(Data.NewValue); \
		} \
	);

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	/* PS */
	GetAuraPS()->OnPlayerLevelChanged.AddLambda(
		[this](int32 Level)
		{
			OnPlayerLevelChanged.Broadcast(Level);
		});
	GetAuraPS()->OnPlayerXPChanged.AddUObject(this, &UOverlayWidgetController::OnPlayerXPChangedReceived);
	GetAuraPS()->OnPlayerAttributePointsChanged.AddLambda(
		[this](int32 AttributePoints)
		{
			OnPlayerAttributePointsChanged.Broadcast(AttributePoints);
		});
	GetAuraPS()->OnPlayerSpellPointsChanged.AddLambda(
		[this](int32 SpellPoints)
		{
			OnPlayerSpellPointsChanged.Broadcast(SpellPoints);
		});
	
	/* Bind Attribute Changes */
	/**
	 * There are two ways I know to bind delegates to functions. Callback and Lambda.
	 * Its mostly just a design choice at this point, I think callback is more readable and understandable,
	 * but that also leads to needing to declare callback functions in the header file and allows others parts
	 * of the class to call those callback functions, which is not always needed.
	 */
	
	/* Lambda function version of binding attributes */
	BIND_CALLBACK(Health);
	BIND_CALLBACK(MaxHealth);
	BIND_CALLBACK(Mana);
	BIND_CALLBACK(MaxMana);
	
	/* Normal version of binding attributes to callback functions */
	/*
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
		.AddUObject(this, &UOverlayWidgetController::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
		.AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
		.AddUObject(this, &UOverlayWidgetController::ManaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
		.AddUObject(this, &UOverlayWidgetController::MaxManaChanged);
	*/

	/* ASC */
	/* Bind GE Asset Tag Applied */
	GetAuraASC()->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// For example, say that Tag = Message.HealthPotion
				// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				bool IsMessageTag = Tag.MatchesTag(MessageTag);

				if (IsMessageTag)
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
	);

	/* Binding Abilities Given */
	if (GetAuraASC()->bStartupAbilitiesGiven)
		BroadcastAbilityInfo();
	
	else
		GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
}

void UOverlayWidgetController::OnPlayerXPChangedReceived(int32 XP)
{
	int32 Level = ULevelUpInfo::FindLevelForXP(GetAuraPS()->LevelUpInfo, XP);
	
	// Get the min and max xp for the current level
	int32 LevelXPFloor = ULevelUpInfo::FindLevelXPFloor(GetAuraPS()->LevelUpInfo, Level);
	int32 LevelXPCeiling = ULevelUpInfo::FindLevelXPCeiling(GetAuraPS()->LevelUpInfo, Level);

	int32 RelativeXPFloor = 0;
	int32 RelativeXPCeiling = LevelXPCeiling - LevelXPFloor;
	int32 RelativeCurrentXP = XP - LevelXPFloor;
	float XPBarPercentage = static_cast<float>(RelativeCurrentXP) / static_cast<float>(RelativeXPCeiling);

	/* Broadcast Delegates */
	OnPlayerXPChanged.Broadcast(RelativeCurrentXP);
	OnPlayerLevelXPFloorChanged.Broadcast(RelativeXPFloor);
	OnPlayerLevelXPCeilingChanged.Broadcast(RelativeXPCeiling);
	OnPlayerXPPercentageChanged.Broadcast(XPBarPercentage);
}

/* These functions only necessary if we are binding attribute change delegates to callback functions */
/*
void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
*/
