// Copyright Druid Mechanics


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(UDataTable* LevelInfoTable, int32 CurrentXP)
{
	int32 CurrentLevel = 1;
	TArray<FName> RowNames = LevelInfoTable->GetRowNames();

	while (CurrentLevel <= RowNames.Num())
	{
		FAuraLevelUpInfo* LevelUpInfo = LevelInfoTable->FindRow<FAuraLevelUpInfo>(RowNames[CurrentLevel - 1], TEXT(""));
		if (LevelUpInfo->LevelUpRequirement <= CurrentXP)
			CurrentLevel++;

		else
			return CurrentLevel;
	}
	
	return CurrentLevel;
}

int32 ULevelUpInfo::FindLevelXPFloor(UDataTable* LevelInfoTable, int32 Level)
{
	if (Level <= 1)
		return 0;

	return GetLevelUpRequirementForLevel(LevelInfoTable, Level - 1);
}

int32 ULevelUpInfo::FindLevelXPCeiling(UDataTable* LevelInfoTable, int32 Level)
{
	if (Level <= 0)
		return 0;
	
	return GetLevelUpRequirementForLevel(LevelInfoTable, Level);
}

int32 ULevelUpInfo::GetAttributePointsForLevel(UDataTable* LevelInfoTable, int32 Level)
{
	TArray<FName> RowNames = LevelInfoTable->GetRowNames();
	if (Level > RowNames.Num())
		return 0;

	FAuraLevelUpInfo* LevelUpInfo = LevelInfoTable->FindRow<FAuraLevelUpInfo>(RowNames[Level - 1], TEXT(""));
	return LevelUpInfo->AttributePointReward;
}

int32 ULevelUpInfo::GetSpellPointsForLevel(UDataTable* LevelInfoTable, int32 Level)
{
	TArray<FName> RowNames = LevelInfoTable->GetRowNames();
	if (Level > RowNames.Num())
		return 0;

	FAuraLevelUpInfo* LevelUpInfo = LevelInfoTable->FindRow<FAuraLevelUpInfo>(RowNames[Level - 1], TEXT(""));
	return LevelUpInfo->SpellPointReward;
}

int32 ULevelUpInfo::GetLevelUpRequirementForLevel(const UDataTable* LevelInfoTable, int32 Level)
{
	FString LevelString = FString::FromInt(Level);
	FAuraLevelUpInfo* LevelUpInfo = LevelInfoTable->FindRow<FAuraLevelUpInfo>(FName(LevelString), TEXT(""));
	
	if (LevelUpInfo == nullptr)
		return 0;

	return LevelUpInfo->LevelUpRequirement;
}
