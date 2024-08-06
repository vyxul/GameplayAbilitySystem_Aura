// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UPassiveNiagaraComponent();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;
	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);

private:
	void BindGameplayTagEvent(UAbilitySystemComponent* InASC);

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
