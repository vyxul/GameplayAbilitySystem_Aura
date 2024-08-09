// Copyright Druid Mechanics


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <Interaction/EnemyInterface.h>

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
		MagicCircle->Destroy();
}

void AAuraPlayerController::SetMaterial(UMaterialInterface* DecalMaterial)
{
	if (IsValid(MagicCircle) && DecalMaterial)
	{
		MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(ACharacter* TargetCharacter, float DamageAmount, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		// Since we are making the object dynamically and out of pre-construct, need to manually register the component
		// CreateDefaultSubObject registers for us in the pre-construct
		DamageText->RegisterComponent();
		// DamageText will play animation right when it is attached
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		// Can detach right away so that the text doesn't follow enemy as anim plays
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		// Set the text on the damage text
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
		return;
	
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);\
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunningAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// crashes if not successful
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	// bind movement keys to Move function
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	// bind shift keys to 2 functions handling the modifier key
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	// bind all the keys inside InputConfig (set up in the editor) to the 3 functions specified
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (GetASC())
	{
		if (GetASC()->HasMatchingGameplayTag(GameplayTags.Player_Block_InputPressed))
			return;

		if (GetASC()->HasMatchingGameplayTag(GameplayTags.StatusEffect_Debuff_Lightning_Stun))
			return;
	}
	
	bAutoRunning = false;
	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

/**
* Gets the first object under a cursor and perform operations on it
*/
void AAuraPlayerController::CursorTrace()
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (GetASC())
	{
		if (GetASC()->HasMatchingGameplayTag(GameplayTags.Player_Block_CursorTrace))
		{
			if (LastActor)
				LastActor->UnHighlightActor();

			if (CurrentActor)
				CurrentActor->UnHighlightActor();

			LastActor = nullptr;
			CurrentActor = nullptr;
			
			return;
		}
		
		if (GetASC()->HasMatchingGameplayTag(GameplayTags.StatusEffect_Debuff_Lightning_Stun))
			return;
	}

	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	// if no hit, return
	if (!CursorHit.bBlockingHit)
		return;

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != CurrentActor)
	{
		if (LastActor)
			LastActor->UnHighlightActor();

		if (CurrentActor)
			CurrentActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (GetASC())
	{
		if (GetASC()->HasMatchingGameplayTag(GameplayTags.Player_Block_InputPressed))
			return;

		if (GetASC()->HasMatchingGameplayTag(GameplayTags.StatusEffect_Debuff_Lightning_Stun))
			return;
	}
	
	// If input was LMB
	if (InputTag.MatchesTagExact(GameplayTags.InputTag_LMB))
	{
		bTargeting = CurrentActor ? true : false;
		bAutoRunning = false;
	}

	if (GetASC())
		GetASC()->AbilityInputTagPressed(InputTag);
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (GetASC())
	{
		if (GetASC()->HasMatchingGameplayTag(GameplayTags.Player_Block_InputReleased))
			return;

		if (GetASC()->HasMatchingGameplayTag(GameplayTags.StatusEffect_Debuff_Lightning_Stun))
			return;
	}
	
	// If input was anything besides LMB
	if (!InputTag.MatchesTagExact(GameplayTags.InputTag_LMB))
	{
		if (GetASC())        
			GetASC()->AbilityInputTagReleased(InputTag);

		return;
	}

	// If input was LMB
	// does not activate ability on release for our implementation, just sets a tag for that ability that it is released
	if (GetASC())        
		GetASC()->AbilityInputTagReleased(InputTag);
	
	// If initial LMB click NOT on enemy and NOT holding shift
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
        		if (FollowTime <= ShortPressThreshold && ControlledPawn)
        		{
        			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
        			{
        				Spline->ClearSplinePoints();
        				for (const FVector& PointLoc : NavPath->PathPoints)
        				{
        					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
        				}

        				if (NavPath->PathPoints.Num() > 0)
        				{
        					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
        					bAutoRunning = true;
        				}
        			}

        			if (GetASC() && !GetASC()->HasMatchingGameplayTag(GameplayTags.Player_Block_InputPressed))
        				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
        		}
        
        		FollowTime = 0.f;
        		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (GetASC())
	{
		if (GetASC()->HasMatchingGameplayTag(GameplayTags.Player_Block_InputHeld))
			return;

		if (GetASC()->HasMatchingGameplayTag(GameplayTags.StatusEffect_Debuff_Lightning_Stun))
			return;
	}
	
	// If input was anything besides LMB
	if (!InputTag.MatchesTagExact(GameplayTags.InputTag_LMB))
	{
		if (GetASC())        
			GetASC()->AbilityInputTagHeld(InputTag);

		return;
	}

	// If input was LMB
	// If LMB on enemy
	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC())        
			GetASC()->AbilityInputTagHeld(InputTag);
	}
	// If LMB NOT on enemy
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit)
			CachedDestination = CursorHit.ImpactPoint;
		

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
	
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	
	return AuraAbilitySystemComponent;
}

