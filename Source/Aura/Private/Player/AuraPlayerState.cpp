// Copyright Druid Mechanics


#include "Player/AuraPlayerState.h"
#include <AbilitySystem/AuraAbilitySystemComponent.h>
#include <AbilitySystem/AuraAttributeSet.h>

#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	BroadcastPlayerLevel();
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	BroadcastPlayerLevel();
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	BroadcastPlayerXP();
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	BroadcastPlayerXP();
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	BroadcastPlayerLevel();
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	BroadcastPlayerXP();
}

void AAuraPlayerState::BroadcastPlayerLevel()
{
	OnPlayerLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::BroadcastPlayerXP()
{
	OnPlayerXPChanged.Broadcast(XP);
}