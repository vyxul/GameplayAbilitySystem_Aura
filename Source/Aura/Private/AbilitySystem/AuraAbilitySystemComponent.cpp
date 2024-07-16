// Copyright Druid Mechanics


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartUpAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			
			GiveAbility(AbilitySpec);
			//GiveAbilityAndActivateOnce(AbilitySpec);
		}
	}
	
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartUpPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
		return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
		return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
				return Tag;
	
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			return Tag;
			
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	// an ability should only have 1 Ability.Status.___ tag at a time
	for (FGameplayTag AbilityTag : AbilitySpec.DynamicAbilityTags)
		if (AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
			return AbilityTag;

	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
			if (Tag.MatchesTagExact(AbilityTag))
				return &AbilitySpec;

	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
		return GetInputTagFromSpec(*AbilitySpec);

	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
		return GetStatusFromSpec(*AbilitySpec);

	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::AbilityHasInputTag(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag)
{
	for (FGameplayTag Tag : AbilitySpec->DynamicAbilityTags)
		if (Tag.MatchesTagExact(InputTag))
			return true;

	return false;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecForInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		if (AbilityHasInputTag(&AbilitySpec, InputTag))
			return &AbilitySpec;

	return nullptr;
}

void UAuraAbilitySystemComponent::ClearInputTag(FGameplayAbilitySpec* AbilitySpec)
{
	const FGameplayTag InputTag = GetInputTagFromSpec(*AbilitySpec);
	AbilitySpec->DynamicAbilityTags.RemoveTag(InputTag);
	MarkAbilitySpecDirty(*AbilitySpec);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		if (AbilityHasInputTag(&AbilitySpec, InputTag))
			ClearInputTag(&AbilitySpec);
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	/* Perform checks before upgrading attribute */
	// avatar actor is a player
	if (!GetAvatarActor()->Implements<UPlayerInterface>())
		return;

	// avatar actor has attribute points to spend
	if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) <= 0)
		return;

	// Passed all the checks, let server handle the upgrade
	ServerUpgradeAttribute(AttributeTag);
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	/* Perform Attribute Upgrade */
	// 1. Upgrade Attribute based on AttributeTag
	FGameplayEventData Payload = FGameplayEventData();
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	
	// 2. Decrement Attribute Points Count on Player State
	if (GetAvatarActor()->Implements<UPlayerInterface>())
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (FAuraAbilityInfo AuraAbilityInfo : AbilityInfo->AbilityInformation)
	{
		if (!AuraAbilityInfo.AbilityTag.IsValid())
			continue;
		
		if (AuraAbilityInfo.LevelRequirement > Level)
			continue;
			
		if (GetSpecFromAbilityTag(AuraAbilityInfo.AbilityTag) == nullptr)
		{
			FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AuraAbilityInfo.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayTags.Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);

			ClientUpdateAbilityStatus(AuraAbilityInfo.AbilityTag, AuraGameplayTags.Abilities_Status_Eligible, 1);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	if (AbilitySpec == nullptr)
		return;

	// Decrement spell points in PS
	if (GetAvatarActor()->Implements<UPlayerInterface>())
		IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
	
	/* At this point, we know that the given AbilityTag is tagged to an ability that is contained in the ASC
	 * StatusTag can be one of 3 options: Eligible, Unlocked, and Equipped
	 * Locked is not possible since that is only given to abilities that the player is not able to unlock yet
	 * and so it would not be in the ASC
	 */
	FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	// 1. Eligible - Change to Unlocked status, level stays at 1
	if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
		AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
		StatusTag = GameplayTags.Abilities_Status_Unlocked;
	}

	// 2 & 3. Unlocked/Equipped - Upgrade the level
	else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		// 2 Ways of leveling an ability
		// 1. Simply add one to the level property in ability spec, does not cancel the ability
		AbilitySpec->Level += 1;
		
		// 2. Remove the ability then give it back with higher level, cancels the ability
		// ... To lazy to add example code, not going with canceling ability anyways
	}

	ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);
	MarkAbilitySpecDirty(*AbilitySpec);
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& InputTag)
{
	/* Cases to worry about
	 *		Need server rpc on AuraASC to make changes
	 *		Need client rpc to show visual changes
	 *		Need to check if given InputTag matches ability type of the given AbilityTag (Offensive/Passive)
	 *		Check if the given AbilityTag has status of Unlocked/Equipped
	 * 
	 * 1. Not equipped yet, equipping to empty slot
	 *		+ InputTag
	 *		+ Equipped
	 *		- Unlocked
	 * 2. Not equipped yet, equipping to occupied slot
	 *		New:
	 *			+ InputTag
	 *			+ Equipped
	 *			- Unlocked
	 *		Old:
	 *			- InputTag
	 *			- Equipped
	 *			+ Unlocked
	 * 3. Equipped in a slot already, equipping to a different empty slot
	 *		- Old InputTag
	 *		+ New InputTag
	 * 4. Equipped already, equipping to same slot
	 *		Do nothing
	 * 5. Equipped in a slot already, equipping to a different occupied slot
	 *		Swap InputTags
	 */
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag& OldInputTag = GetInputTagFromSpec(*AbilitySpec);
		FGameplayTag NewInputTag = InputTag;
		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);

		// 4
		if (OldInputTag.MatchesTagExact(InputTag))
			return;
		
		FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
		const bool bAbilityUnlocked = (StatusTag == AuraGameplayTags.Abilities_Status_Unlocked);
		const bool bAbilityEquipped = (StatusTag == AuraGameplayTags.Abilities_Status_Equipped);
		const bool bStatusValid = bAbilityUnlocked || bAbilityEquipped;
		
		if (!bStatusValid)
			return;

		FGameplayAbilitySpec* OtherAbilitySpec = GetSpecForInputTag(InputTag);
		
		// Ability should always be either Unlocked or Equipped by this point, not both
		// 1 & 2
		if (bAbilityUnlocked)
		{
			// 1
			if (!OtherAbilitySpec)
			{
				AbilitySpec->DynamicAbilityTags.AddTag(InputTag);
				AbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags.Abilities_Status_Equipped);
			}
			
			// 2
			else
			{
				AbilitySpec->DynamicAbilityTags.AddTag(InputTag);
				AbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags.Abilities_Status_Equipped);

				
				OtherAbilitySpec->DynamicAbilityTags.RemoveTag(InputTag);
				OtherAbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags.Abilities_Status_Unlocked);
				OtherAbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags.Abilities_Status_Equipped);
			}
		}

		// 3 & 5
		else if (bAbilityEquipped)
		{
			// 3
			if (!OtherAbilitySpec)
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(OldInputTag);
				AbilitySpec->DynamicAbilityTags.AddTag(InputTag);
			}

			// 5
			else
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(OldInputTag);
				AbilitySpec->DynamicAbilityTags.AddTag(InputTag);

				OtherAbilitySpec->DynamicAbilityTags.RemoveTag(InputTag);
				OtherAbilitySpec->DynamicAbilityTags.AddTag(OldInputTag);
			}
		}

		MarkAbilitySpecDirty(*AbilitySpec);
		ClientEquipAbility(AbilityTag, StatusTag, NewInputTag, OldInputTag);

		if (OtherAbilitySpec)
		{
			FGameplayTag OtherAbilityTag = GetAbilityTagFromSpec(*OtherAbilitySpec);
			FGameplayTag OtherStatusTag = GetStatusFromSpec(*OtherAbilitySpec);
			MarkAbilitySpecDirty(*OtherAbilitySpec);
			ClientEquipAbility(OtherAbilityTag, OtherStatusTag, OldInputTag, FGameplayTag());
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,	const FGameplayTag& StatusTag, const FGameplayTag& NewInputTag, const FGameplayTag& OldInputTag)
{
	/* 1. Empty -> Empty: Good
	 * 2. Empty -> Occupied: 
	 * 3. Occupied -> Empty
	 * 4. Occupied -> Same
	 * 5. Occupied -> Other Occupied
	 * 
	 */
	FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
	AbilitySlotInfo.Broadcast(AbilityTag, StatusTag, NewInputTag);

	if (OldInputTag != FGameplayTag::EmptyTag)
	{
		AbilitySlotInfo.Broadcast(AuraGameplayTags.Abilities_None, FGameplayTag(), OldInputTag);
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FAuraAbilityInfo& AuraAbilityInfo, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AuraAbilityInfo.AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraAbility->GetFormattedDescription(AuraAbility, AbilitySpec->Level, AuraAbilityInfo.Description);
			OutNextLevelDescription = AuraAbility->GetFormattedDescription(AuraAbility, AbilitySpec->Level + 1, AuraAbilityInfo.Description);
			return true;
		}
	}

	// If Ability not found in ASC, it is still in Locked status
	OutDescription = UAuraGameplayAbility::GetLockedDescription(AuraAbilityInfo.LevelRequirement);
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	// Get AbilityTag, StatusTag, and AbilityLevel to broadcast
	// AbilityTag
	FGameplayTag AbilityTag = FGameplayTag();
	for (FGameplayTag Tag : AbilitySpec.Ability->AbilityTags)
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			AbilityTag = Tag;

	// StatusTag
	FGameplayTag StatusTag = FGameplayTag();
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
			StatusTag = Tag;

	// AbilityLevel
	const int32 AbilityLevel = AbilitySpec.Level;
	
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}
