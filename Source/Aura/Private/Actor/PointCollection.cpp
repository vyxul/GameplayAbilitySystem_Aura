// Copyright Druid Mechanics


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	/* Creating Pts and adding to array */
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	// 1
	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	ImmutablePts.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	// 2
	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePts.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	// 3
	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePts.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	// 4
	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePts.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	// 5
	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePts.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	// 6
	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePts.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	// 7
	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePts.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	// 8
	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePts.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	// 9
	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePts.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	// 10
	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());

}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,
	float YawOverride)
{
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds."))

	TArray<USceneComponent*> ArrayCopy;
	for (USceneComponent* Pt : ImmutablePts)
	{
		if (ArrayCopy.Num() >= NumPoints)
			return ArrayCopy;

		FVector OriginLocation = Pt_0->GetComponentLocation();
		FVector PointLocation = Pt->GetComponentLocation();
		FVector AdjustedLocation = PointLocation;
		if (Pt != Pt_0)
		{
			FVector ToTarget = PointLocation - OriginLocation;
			ToTarget = ToTarget.RotateAngleAxis(YawOverride, FVector::UpVector);
			AdjustedLocation = OriginLocation + ToTarget;
			Pt->SetWorldLocation(OriginLocation + ToTarget);
		}

		FHitResult HitResult;
		const FVector RaisedLocation = FVector(AdjustedLocation.X, AdjustedLocation.Y, AdjustedLocation.Z + 500);
		const FVector LoweredLocation = FVector(AdjustedLocation.X, AdjustedLocation.Y, AdjustedLocation.Z - 500);
		TArray<AActor*> ActorsAtLocation;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			ActorsAtLocation,
			TArray<AActor*>(),
			1500.f,
			GetActorLocation());
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(ActorsAtLocation);

		GetWorld()->LineTraceSingleByProfile(
			HitResult,
			RaisedLocation,
			LoweredLocation,
			FName("BlockAll"),
			QueryParams);

		AdjustedLocation.Z = HitResult.ImpactPoint.Z;
		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));
		
		ArrayCopy.Add(Pt);
	}

	return ArrayCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}
