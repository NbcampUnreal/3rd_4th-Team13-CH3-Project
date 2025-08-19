#include "UI/Widget/GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Characters/MainPlayerController.h"
#include "GameFramework/MatrixGameState.h"


void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();	

	if (Button_Title)
	{
		Button_Title->OnClicked.AddDynamic(this, &UGameOverWidget::OnTitleClicked);
		Button_Title->OnHovered.AddDynamic(this, &UGameOverWidget::OnTitleFocused);
		Button_Title->OnUnhovered.AddDynamic(this, &UGameOverWidget::OnTitleUnfocused);
	}

	if (Button_Quit)
	{
		Button_Quit->OnClicked.AddDynamic(this, &UGameOverWidget::OnQuitClicked);
		Button_Quit->OnHovered.AddDynamic(this, &UGameOverWidget::OnQuitFocused);
		Button_Quit->OnUnhovered.AddDynamic(this, &UGameOverWidget::OnQuitUnfocused);
	}

	if (LogoAnim)
	{
		PlayAnimation(LogoAnim);
	}

	if(AMatrixGameState* GS = GetWorld()->GetGameState<AMatrixGameState>())
	{
		int32 CurrentWave = GS->CurrentWave;
		int32 TotalKillCount = GS->KillCount;
		FString WaveText = FString::Printf(TEXT("Wave %d"), CurrentWave);
		FString KillCount = FString::Printf(TEXT("%d"), TotalKillCount);
		if (Text_Wave)
		{
			Text_Wave->SetText(FText::FromString(WaveText));
		}
		if (Text_TotalCount)
		{
			Text_TotalCount->SetText(FText::FromString(KillCount));
			Text_TotalCount->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f)));
			PlayAnimation(PunchCount, 0.f, 0);
		}
	}
}

void UGameOverWidget::OnTitleClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if(AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
		{
			MPC->GoToMainMenu(); 
		}
	}
}

void UGameOverWidget::OnQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
		{
			MPC->OnMenuExit(); 
		}
	}
}

void UGameOverWidget::OnTitleFocused()
{
	if (Br_Title)
	{
		Br_Title->SetBrushColor(FLinearColor::Red);
	}
}

void UGameOverWidget::OnQuitFocused()
{
	if (Br_Quit)
	{
		Br_Quit->SetBrushColor(FLinearColor::Red);
	}
}

void UGameOverWidget::OnTitleUnfocused()
{
	if (Br_Title)
	{
		Br_Title->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}

void UGameOverWidget::OnQuitUnfocused()
{
	if (Br_Quit)
	{
		Br_Quit->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
}
