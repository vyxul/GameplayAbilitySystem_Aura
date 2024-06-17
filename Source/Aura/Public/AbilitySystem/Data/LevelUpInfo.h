// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT()
struct FAuraLevelUpInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttributePointReward = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SpellPointReward = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Level Up Info")
	TObjectPtr<UDataTable> LevelUpInformation;
	
	/* This is assuming that all the level up info in the LevelUpInformation array is sorted in order based on LevelUpRequirements */
	UFUNCTION(BlueprintCallable)
	int32 FindLevelForXP(int32 CurrentXP);
};
