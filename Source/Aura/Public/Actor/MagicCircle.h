// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagicCircle();
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UDecalComponent> MagicCircleDecal;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
