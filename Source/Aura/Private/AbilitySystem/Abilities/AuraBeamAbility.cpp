// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraBeamAbility.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamAbility::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamAbility::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamAbility::TraceFirstTarget(const FVector& BeamStartLocation, const FVector& BeamTargetLocation, const float TraceRadius)
{
	check(OwnerCharacter);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter);

	FHitResult HitResult;
	
	UKismetSystemLibrary::SphereTraceSingle(
		OwnerCharacter,
		BeamStartLocation,
		BeamTargetLocation,
		TraceRadius,
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
}

void UAuraBeamAbility::StoreSecondaryTargets(TArray<AActor*>& OutSecondaryTargets, float Radius)
{
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		Radius,
		MouseHitActor->GetActorLocation());

	TArray<AActor*> SecondaryTargets;
	UAuraAbilitySystemLibrary::GetClosestTargets(
		MaxSecondaryTargets,
		OverlappingActors,
		SecondaryTargets,
		MouseHitActor->GetActorLocation());

	OutSecondaryTargets = SecondaryTargets;
}
