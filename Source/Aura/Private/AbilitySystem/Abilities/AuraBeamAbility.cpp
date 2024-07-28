// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraBeamAbility.h"

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

void UAuraBeamAbility::StoreOwnerPlayerController()
{
	if (CurrentActorInfo)
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
}
