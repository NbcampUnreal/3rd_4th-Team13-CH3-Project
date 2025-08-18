#include "UI/Widget/PauseMenuWidget.h"
#include "Characters/MainPlayerController.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Transform_GuidanceText)
	{
		PlayAnimation(Transform_GuidanceText, 0.f, 0);
	}

	SetIsFocusable(true);
	SetKeyboardFocus();

	if (Button_Continue) { Button_Continue->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleContinueClicked); }
	if (Button_Options) { Button_Options->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleOptionsClicked); }
	if (Button_MainMenu) { Button_MainMenu->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleMainMenuClicked); }

}

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		RequestResume();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPauseMenuWidget::RequestResume()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(PlayerController))
		{
			MainPlayerController->HandlePauseMenu();
		}
	}
}

void UPauseMenuWidget::HandleContinueClicked()
{
	RequestResume();
}

void UPauseMenuWidget::HandleOptionsClicked()
{
	OnPauseMenuOptions.Broadcast(EGameState::Paused);
}

void UPauseMenuWidget::HandleMainMenuClicked()
{
	OnPauseMenuMainMenu.Broadcast();
}
