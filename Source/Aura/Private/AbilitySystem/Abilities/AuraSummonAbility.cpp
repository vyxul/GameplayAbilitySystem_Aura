// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	if (NumMinions <= 0)
		return TArray<FVector>();
	
	const AActor* Owner = GetAvatarActorFromActorInfo();
	const FVector OwnerForward = Owner->GetActorForwardVector();
	const FVector OwnerLocation = Owner->GetActorLocation();
	const float DeltaSpread = NumMinions > 1 ? SpawnSpread / (NumMinions - 1) : SpawnSpread / 2;
	TArray<FVector> SpawnLocations;
	float DebugShapeDuration = 1.5f;

	const FVector LeftEdgeVector = OwnerForward.RotateAngleAxis(-(SpawnSpread / 2.f), FVector::UpVector);
	// const FVector RightEdgeVector = OwnerForward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);

	// // Debug Shapes
	// // Left Edge
	// UKismetSystemLibrary::DrawDebugArrow(Owner, OwnerLocation, OwnerLocation + (LeftEdgeVector * MaxSpawnDistance), 4.f, FLinearColor::Red, DebugShapeDuration);
	// DrawDebugSphere(GetWorld(), OwnerLocation + (LeftEdgeVector * MinSpawnDistance), 15.f, 12, FColor::Red, false, DebugShapeDuration);
	// DrawDebugSphere(GetWorld(), OwnerLocation + (LeftEdgeVector * MaxSpawnDistance), 15.f, 12, FColor::Red, false, DebugShapeDuration);
	// // RightEdge Edge
	// UKismetSystemLibrary::DrawDebugArrow(Owner, OwnerLocation, OwnerLocation + (RightEdgeVector * MaxSpawnDistance), 4.f, FLinearColor::Green, DebugShapeDuration);
	// DrawDebugSphere(GetWorld(), OwnerLocation + (RightEdgeVector * MinSpawnDistance), 15.f, 12, FColor::Green, false, DebugShapeDuration);
	// DrawDebugSphere(GetWorld(), OwnerLocation + (RightEdgeVector * MaxSpawnDistance), 15.f, 12, FColor::Green, false, DebugShapeDuration);

	for (int32 i = 0; i < NumMinions; i++)
	{
		FVector Direction = LeftEdgeVector.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);

		if (NumMinions == 1)
			Direction = Direction.RotateAngleAxis(DeltaSpread, FVector::UpVector);
				
		UKismetSystemLibrary::DrawDebugArrow(Owner, OwnerLocation + (Direction * MinSpawnDistance), OwnerLocation + (Direction * MaxSpawnDistance), 4.f, FLinearColor::Blue, DebugShapeDuration);
		const FVector ChosenSpawnLocation = OwnerLocation + (Direction * (FMath::RandRange(MinSpawnDistance, MaxSpawnDistance)));
		DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 15.f, 12, FColor::Blue, false, DebugShapeDuration);
		SpawnLocations.Add(ChosenSpawnLocation);
	}
	
	return SpawnLocations;
}
