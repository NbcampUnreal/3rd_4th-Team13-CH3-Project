#include "UI/Widget/PauseMenuWidget.h"
#include "Characters/MainPlayerController.h"

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("ESC pressed in PauseMenu"));

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (APlayerController* PlayerController = GetOwningPlayer())
		{
			if (AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(PlayerController))
			{
				MainPlayerController->HandlePauseMenu();
			}
		}

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}