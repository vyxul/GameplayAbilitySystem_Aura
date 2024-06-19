// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStatsChangedSignature, int32);

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Level")
	TObjectPtr<UDataTable> LevelUpInfo;

	/* Delegates */
	FPlayerStatsChangedSignature OnPlayerLevelChanged;
	FPlayerStatsChangedSignature OnPlayerXPChanged;
	FPlayerStatsChangedSignature OnPlayerAttributePointsChanged;
	FPlayerStatsChangedSignature OnPlayerSpellPointsChanged;

	/* Getters and Setters */
	// Level
	UFUNCTION(BlueprintCallable, Category= "Player Properties | Level")
	int32 GetPlayerLevel() const;
	
	UFUNCTION(BlueprintCallable, Category= "Player Properties | Level")
	void SetLevel(int32 InLevel);
	
	UFUNCTION(BlueprintCallable, Category= "Player Properties | Level")
	void AddToLevel(int32 InLevel);

	// XP
	UFUNCTION(BlueprintCallable, Category= "Player Properties | XP")
	int32 GetXP() const;

	UFUNCTION(BlueprintCallable, Category= "Player Properties | XP")
	void SetXP(int32 InXP);

	UFUNCTION(BlueprintCallable, Category= "Player Properties | XP")
	void AddToXP(int32 InXP);

	// Attribute Points
	UFUNCTION(BlueprintCallable, Category= "Player Properties | Attribute Points")
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintCallable, Category= "Player Properties | Attribute Points")
	void SetAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintCallable, Category= "Player Properties | Attribute Points")
	void AddToAttributePoints(int32 InAttributePoints);
	
	// Spell Points
	UFUNCTION(BlueprintCallable, Category= "Player Properties | Spell Points")
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintCallable, Category= "Player Properties | Spell Points")
	void SetSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintCallable, Category= "Player Properties | Spell Points")
	void AddToSpellPoints(int32 InSpellPoints);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_XP)
	int32 XP = 0;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);

	// Helper function to broadcast delegate in multiple areas
	void BroadcastPlayerLevel();
	void BroadcastPlayerXP();
	void BroadcastAttributePoints();
	void BroadcastSpellPoints();
};
