#include "Characters/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

AMainPlayerController::AMainPlayerController()
	: InputMappingContext(nullptr), 
	MoveAction(nullptr), 
	JumpAction(nullptr),
	LookAction(nullptr), 
	ShootAction(nullptr), 
	InteractAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr)
{

}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UMainHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
}

/*void AMainPlayerController::NotifyAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}*/