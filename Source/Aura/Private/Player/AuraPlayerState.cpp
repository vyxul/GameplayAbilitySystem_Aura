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
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

/* Getters and Setters */
// Level
int32 AAuraPlayerState::GetPlayerLevel() const
{
	return Level;
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

// XP
int32 AAuraPlayerState::GetXP() const
{
	return XP;
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

// Attribute Points
int32 AAuraPlayerState::GetAttributePoints() const
{
	return AttributePoints;
}

void AAuraPlayerState::SetAttributePoints(int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	BroadcastAttributePoints();
}

void AAuraPlayerState::AddToAttributePoints(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;
	BroadcastAttributePoints();
}

// Spell Points
int32 AAuraPlayerState::GetSpellPoints() const
{
	return SpellPoints;
}

void AAuraPlayerState::SetSpellPoints(int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	BroadcastSpellPoints();
}

void AAuraPlayerState::AddToSpellPoints(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;
	BroadcastSpellPoints();
}

/* OnRep_Notify Functions */
void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	BroadcastPlayerLevel();
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	BroadcastPlayerXP();
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	BroadcastAttributePoints();
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	BroadcastSpellPoints();
}

/* Broadcast Helper functions */
void AAuraPlayerState::BroadcastPlayerLevel()
{
	OnPlayerLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::BroadcastPlayerXP()
{
	OnPlayerXPChanged.Broadcast(XP);
}

void AAuraPlayerState::BroadcastAttributePoints()
{
	OnPlayerAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::BroadcastSpellPoints()
{
	OnPlayerSpellPointsChanged.Broadcast(SpellPoints);
}
