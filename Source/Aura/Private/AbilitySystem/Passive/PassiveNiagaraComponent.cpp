// Copyright Druid Mechanics


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());

	if (OwnerASC)
		BindGameplayTagEvent(OwnerASC);
	
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
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UPassiveNiagaraComponent::OnOwnerDeath);
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
			Activate();
		
		else
			Deactivate();
	}
}

void UPassiveNiagaraComponent::BindGameplayTagEvent(UAbilitySystemComponent* InASC)
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
}

void UPassiveNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
