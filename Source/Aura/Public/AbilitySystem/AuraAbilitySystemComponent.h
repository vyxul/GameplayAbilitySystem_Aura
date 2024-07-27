// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FAuraAbilityInfo;
class UAbilityInfo;
/* Gameplay Effect Delegates */
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */, int32 /* AbilityLevel */)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilitySlotInfo, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */, const FGameplayTag& /* InputTag */);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/** Called by other classes when ASC is set up. This function will bind delegates to callback functions. */
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilitySlotInfo AbilitySlotInfo;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
	/* Returns ability spec that has matching ability tag with passed in AbilityTag within the ASC*/
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromSpec(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);

	static bool AbilityHasInputTag(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag);
	FGameplayAbilitySpec* GetSpecForInputTag(const FGameplayTag& InputTag);
	void ClearInputTag(FGameplayAbilitySpec* AbilitySpec);
	void ClearAbilitiesOfInputTag(const FGameplayTag& InputTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatuses(int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& NewInputTag, const FGameplayTag& OldInputTag);

	bool GetDescriptionsByAbilityTag(const FAuraAbilityInfo& AuraAbilityInfo, FString& OutDescription, FString& OutNextLevelDescription);
	
protected:
	virtual void OnRep_ActivateAbilities() override;

	void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
