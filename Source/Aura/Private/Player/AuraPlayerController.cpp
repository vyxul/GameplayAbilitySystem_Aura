// Copyright Druid Mechanics


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <Interaction/EnemyInterface.h>

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
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

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
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
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	// if no hit, return
	if (!CursorHit.bBlockingHit) return;

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	* Line trace from cursor. There are several scenarios:
	*  A. LastActor is null && CurrentActor is null
	*		- Do nothing
	*  B. LastActor is null && CurrentActor is valid
	*		- Call Highlight function on CurrentActor
	*  C. LastActor is valid && CurrentActor is null
	*		- Call UnHighlight function on LastActor
	*  D. Both actors are valid, but LastActor != CurrentActor
	*		- UnHighlight LastActor, Highlight CurrentActor
	*  E. Both actors are valid, LastActor == CurrentActor
	*		- Do nothing
	*/

	// Scenarios A and B
	if (LastActor == nullptr)
	{
		// A
		// dont need any code for this scenario

		// B
		if (CurrentActor != nullptr)
		{
			CurrentActor->HighlightActor();
		}
	}

	// Scenarios C, D, E
	else // LastActor is valid
	{
		// C
		if (CurrentActor == nullptr)
		{
			LastActor->UnHighlightActor();
		}

		// D and E
		else // CurrentActor is valid
		{
			// D
			if (LastActor != CurrentActor)
			{
				LastActor->UnHighlightActor();
				CurrentActor->HighlightActor();
			}

			// E
			// dont need any code for this scenario
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());

	if (GetASC() == nullptr)
		return;
	
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());
	
	if (GetASC() == nullptr)
		return;
	
	GetASC()->AbilityInputTagHeld(InputTag);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AuraAbilitySystemComponent;
}
