// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

enum class ECharacterClass : uint8;
class AAuraEnemy;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Enemy Info")
	TSubclassOf<AAuraEnemy> EnemyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Enemy Info")
	int32 EnemyLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Enemy Info")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
};
