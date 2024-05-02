// Copyright Druid Mechanics


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		//  Bind the callback functions for OnAttributeChange
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);

		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}

	return AttributeMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	/* Setting up Overlay Widget and Controller
	 * Overlay widget will be created on start and paired up with controller
	 */
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitalized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

	// 1. Create the overlay widget
	UUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	// 2. Create the widget controller
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	GetOverlayWidgetController(WidgetControllerParams);

	// 3. Set up the one way connection from OverlayWidget to the WidgetController
	OverlayWidget->SetWidgetController(OverlayWidgetController);

	// 4. Broadcast initial values for setting up correct info
	OverlayWidgetController->BroadcastInitialValues();

	// Finally, add the widget to the viewport
	Widget->AddToViewport();

	/* Setting up Attribute Menu Widget Controller
	 * Only Controller is needed, the actual widget will be created dynamically from Overlay widget
	 * Since this is the case, don't need extra steps to link widget and controller, just set up properties
	 * Don't need to broadcast initial values since no widgets are linked yet, widgets can call that on their own
	 */
	checkf(AttributeMenuWidgetControllerClass, TEXT("Attribute Menu Widget Controller Class uninitialized, please fill out BP_AuraHUD"));
	
	// Create the widget controller, reusing the params from Overlay section
	GetAttributeMenuWidgetController(WidgetControllerParams);
	
}
