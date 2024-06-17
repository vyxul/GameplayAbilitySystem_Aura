// Copyright Druid Mechanics


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 CurrentXP)
{
	int32 CurrentLevel = 1;
	TArray<FName> RowNames = LevelUpInformation->GetRowNames();

	while (CurrentLevel <= RowNames.Num())
	{
		FAuraLevelUpInfo* LevelUpInfo = LevelUpInformation.Get()->FindRow<FAuraLevelUpInfo>(RowNames[CurrentLevel - 1], TEXT(""));
		if (LevelUpInfo->LevelUpRequirement <= CurrentXP)
			CurrentLevel++;

		else
			return CurrentLevel;
	}
	
	return CurrentLevel;
}
