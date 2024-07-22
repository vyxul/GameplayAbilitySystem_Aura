// Copyright Druid Mechanics


#include "AbilitySystem/Effects/StatusEffects/StatusEffectNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UStatusEffectNiagaraComponent::UStatusEffectNiagaraComponent()
{
	bAutoActivate = false;
}

void UStatusEffectNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	// Dealing with StatusEffectTag
	if (OwnerASC)
	{
		BindGameplayTagEvent(OwnerASC);
	}
	else if (CombatInterface)
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this,
			[this](UAbilitySystemComponent* InASC)
			{
				BindGameplayTagEvent(InASC);
			});
	}

	// Dealing with OnDeath
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UStatusEffectNiagaraComponent::OnOwnerDeath);
	}
}

void UStatusEffectNiagaraComponent::StatusEffectTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
		Activate();
	
	else
		Deactivate();
}

void UStatusEffectNiagaraComponent::BindGameplayTagEvent(UAbilitySystemComponent* InASC)
{
	InASC->RegisterGameplayTagEvent(StatusEffectTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UStatusEffectNiagaraComponent::StatusEffectTagChanged);
}

void UStatusEffectNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
