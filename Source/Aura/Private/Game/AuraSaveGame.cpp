// Copyright Druid Mechanics


#include "Game/AuraSaveGame.h"

FSavedMap UAuraSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
	{
		if (Map.MapAssetName == InMapName)
			return Map;
	}
	
	return FSavedMap();
}

bool UAuraSaveGame::HasMap(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
	{
		if (Map.MapAssetName == InMapName)
			return true;
	}
	
	return false;
}
