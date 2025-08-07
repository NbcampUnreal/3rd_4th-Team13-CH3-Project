#include "Characters/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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

	if (WeaponHUDWidgetClass)
	{
		WeaponHUDWidgetInstance = CreateWidget<UWeaponHUDWidget>(this, WeaponHUDWidgetClass);
		if (WeaponHUDWidgetInstance)
		{
			WeaponHUDWidgetInstance->AddToViewport();
		}
	}
}

void AMainPlayerController::NotifyAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pause", IE_Pressed, this, &AMainPlayerController::HandlePauseMenu);
}

void AMainPlayerController::HandlePauseMenu()
{
	if (!bIsPaused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		bIsPaused = true;

		if (PauseMenuClass)
		{
			PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
			if (PauseMenuInstance)
			{
				PauseMenuInstance->AddToViewport();

				PauseMenuInstance->SetIsFocusable(true);
				PauseMenuInstance->bIsFocusable = true;	
				PauseMenuInstance->SetKeyboardFocus();

				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputMode);
				bShowMouseCursor = true;
			}
		}
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		bIsPaused = false;

		if (PauseMenuInstance)
		{
			PauseMenuInstance->RemoveFromParent();
			PauseMenuInstance = nullptr;
		}

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}