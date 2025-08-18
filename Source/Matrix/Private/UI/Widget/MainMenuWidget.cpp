#include "UI/Widget/MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/MatrixCoreTypes.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_StartGame)	{ Button_StartGame->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleStartClicked); }
	if (Button_Options)	{ Button_Options->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleOptionsClicked); }
	if (Button_Exit) { Button_Exit->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleExitClicked); }

	if (Button_StartGame) { Button_StartGame->SetKeyboardFocus(); }

	if (MenuMediaPlayer && MenuMediaSource)
	{
		MenuMediaPlayer->SetLooping(true);
		if (MenuMediaPlayer->OpenSource(MenuMediaSource))
		{
			MenuMediaPlayer->Play();
		}
	}
}

void UMainMenuWidget::NativeDestruct()
{
	if (MenuMediaPlayer)
	{
		MenuMediaPlayer->Close();
	}
	Super::NativeDestruct();
}

void UMainMenuWidget::HandleStartClicked()
{
	OnStartRequested.Broadcast();
}

void UMainMenuWidget::HandleOptionsClicked()
{
	OnOptionsRequested.Broadcast(EGameState::MainMenu);
}

void UMainMenuWidget::HandleExitClicked()
{
	OnExitRequested.Broadcast();
}
