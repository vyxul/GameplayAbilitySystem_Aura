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

	FPlayerStatsChangedSignature OnPlayerLevelChanged;
	FPlayerStatsChangedSignature OnPlayerXPChanged;

	/* Level */
	UFUNCTION(BlueprintCallable, Category= "Level")
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	
	UFUNCTION(BlueprintCallable, Category= "Level")
	void SetLevel(int32 InLevel);
	
	UFUNCTION(BlueprintCallable, Category= "Level")
	void AddToLevel(int32 InLevel);

	/* XP */
	UFUNCTION(BlueprintCallable, Category= "XP")
	FORCEINLINE int32 GetXP() const { return XP; }

	UFUNCTION(BlueprintCallable, Category= "XP")
	void SetXP(int32 InXP);

	UFUNCTION(BlueprintCallable, Category= "XP")
	void AddToXP(int32 InXP);

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

	// Helper function to broadcast delegate in multiple areas
	void BroadcastPlayerLevel();
	void BroadcastPlayerXP();
};
