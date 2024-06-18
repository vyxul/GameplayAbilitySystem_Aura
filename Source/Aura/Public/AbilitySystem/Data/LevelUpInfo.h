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
UCLASS(BlueprintType, Blueprintable)
class AURA_API ULevelUpInfo : public UObject
{
	GENERATED_BODY()

public:
	/* This is assuming that all the level up info in the LevelUpInformation array is sorted in order based on LevelUpRequirements */
	UFUNCTION(BlueprintCallable)
	static int32 FindLevelForXP(UDataTable* LevelInfoTable, int32 CurrentXP);

	UFUNCTION(BlueprintCallable)
	static int32 FindLevelXPFloor(UDataTable* LevelInfoTable, int32 Level);

	UFUNCTION(BlueprintCallable)
	static int32 FindLevelXPCeiling(UDataTable* LevelInfoTable, int32 Level);

	UFUNCTION(BlueprintCallable)
	static int32 GetAttributePointsForLevel(UDataTable* LevelInfoTable, int32 Level);
	
	UFUNCTION(BlueprintCallable)
	static int32 GetSpellPointsForLevel(UDataTable* LevelInfoTable, int32 Level);

private:
	/* Helper function for finding xp info for a level */
	static int32 GetLevelUpRequirementForLevel(const UDataTable* LevelInfoTable, int32 Level);
};
