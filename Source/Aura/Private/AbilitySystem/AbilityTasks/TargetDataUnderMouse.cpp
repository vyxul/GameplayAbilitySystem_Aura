// Copyright Druid Mechanics


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return  MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
		SendMouseCursorData();

	else
	{
		//TODO: We are on the server, so listen for Target Data
	}
	
	
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// Create Scoped Prediction Window for use with sending data and prediction, makes everything within this scope predicted
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	// Get the HitResult of cursor from PC
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    FHitResult CursorHit;
    PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// Prepare the target data to send to server
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);
	
	// Send target data to server
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	// Broadcast delegate for local client
	if (ShouldBroadcastAbilityTaskDelegates())
		ValidData.Broadcast(DataHandle);
}
