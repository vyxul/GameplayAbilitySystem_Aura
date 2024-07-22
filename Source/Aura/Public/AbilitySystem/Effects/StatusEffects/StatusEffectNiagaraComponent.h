// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "StatusEffectNiagaraComponent.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UStatusEffectNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
    UStatusEffectNiagaraComponent();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StatusEffectTag;

protected:
	virtual void BeginPlay() override;
	void StatusEffectTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

private:
	void BindGameplayTagEvent(UAbilitySystemComponent* InASC);

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
