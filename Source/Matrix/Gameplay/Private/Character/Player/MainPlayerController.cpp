#include "MainPlayerController.h"
#include "EnhancedInputSubsystems.h"

AMainPlayerController::AMainPlayerController()
	: InputMappingContext(nullptr), MoveAction(nullptr), JumpAction(nullptr),
	LookAction(nullptr), ShootAction(nullptr), InteractAction(nullptr)
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
}
