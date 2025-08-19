#include "UI/Widget/GameClearWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Characters/MainPlayerController.h"
#include "GameFramework/MatrixGameState.h"

void UGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Title)
	{
		Button_Title->OnClicked.AddDynamic(this, &UGameClearWidget::OnTitleClicked);
		Button_Title->OnHovered.AddDynamic(this, &UGameClearWidget::OnTitleFocused);
		Button_Title->OnUnhovered.AddDynamic(this, &UGameClearWidget::OnTitleUnfocused);
	}

	if (Button_Quit)
	{
		Button_Quit->OnClicked.AddDynamic(this, &UGameClearWidget::OnQuitClicked);
		Button_Quit->OnHovered.AddDynamic(this, &UGameClearWidget::OnQuitFocused);
		Button_Quit->OnUnhovered.AddDynamic(this, &UGameClearWidget::OnQuitUnfocused);
	}

	if (LogoAnim)
	{
		PlayAnimation(LogoAnim);
	}

	if (AMatrixGameState* GS = GetWorld()->GetGameState<AMatrixGameState>())
	{
		int32 CurrentWave = GS->CurrentWave;
		int32 TotalKillCount = GS->KillCount;
		FString WaveText = FString::Printf(TEXT("Wave %d"), CurrentWave);
		FString KillCount = FString::Printf(TEXT("%d"), TotalKillCount);
		if (Text_Wave)
		{
			Text_Wave->SetText(FText::FromString(WaveText));
		}
		if(Text_TotalCount)
		{
			Text_TotalCount->SetText(FText::FromString(KillCount));
			Text_TotalCount->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 1.f, 0.f, 1.f)));
			PlayAnimation(PunchCount, 0.f, 0);
		}
	}

}

void UGameClearWidget::OnTitleClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
		{
			MPC->GoToMainMenu();
		}
	}
}

void UGameClearWidget::OnQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
		{
			MPC->OnMenuExit();
		}
	}
}

void UGameClearWidget::OnTitleFocused()
{
	if (Br_Title)
	{	
		Br_Title->SetBrushColor(FLinearColor(0.f, 1.f, 0.f, 1.f));
	}
}

void UGameClearWidget::OnQuitFocused()
{
	if (Br_Quit)
	{
		Br_Quit->SetBrushColor(FLinearColor(0.f, 1.f, 0.f, 1.f));
	}
}

void UGameClearWidget::OnTitleUnfocused()
{
	if (Br_Title)
	{
		Br_Title->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}

void UGameClearWidget::OnQuitUnfocused()
{
	if (Br_Quit)
	{
		Br_Quit->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}
