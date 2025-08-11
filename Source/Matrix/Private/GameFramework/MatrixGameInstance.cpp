#include "GameFramework/MatrixGameInstance.h"

#include "Kismet/GameplayStatics.h"

void UMatrixGameInstance::AddScore(int32 ScoreToAdd)
{
	PlayerScore += ScoreToAdd;
	UE_LOG(LogTemp, Warning, TEXT("Current Score : %d"), PlayerScore);
}

void UMatrixGameInstance::ResetGameData()
{
	PlayerScore = 0;
	CurrentLevel = 1;
	UE_LOG(LogTemp, Warning, TEXT("Game data has been reset."));
}

void UMatrixGameInstance::AdvanceToNextLevel()
{
	if (CurrentLevel < MaxLevel)
	{
		CurrentLevel++;
		UE_LOG(LogTemp, Warning, TEXT("Preparing to move to level %d"), CurrentLevel);
	}
	else
	{
		// 보스전 진입 시
		UE_LOG(LogTemp, Warning, TEXT("All levels cleared! Preparing for Boss Fight."));
	}
}

void UMatrixGameInstance::StartGame()
{
	if (!MainGameLevelName.IsNone())
	{
		// TODO: 여기에 로딩 스크린 UI를 띄우는 로직 추가
		UE_LOG(LogTemp, Warning, TEXT("Starting Game... Loading Level: %s"), *MainGameLevelName.ToString());
		UGameplayStatics::OpenLevel(this, MainGameLevelName);
	}
}

void UMatrixGameInstance::GoToMainMenu()
{
	if (!MainMenuLevelName.IsNone())
	{
		// TODO: 로딩 스크린 UI 띄우기
		UE_LOG(LogTemp, Warning, TEXT("Returning to Main Menu... Loading Level: %s"), *MainMenuLevelName.ToString());
		UGameplayStatics::OpenLevel(this, MainMenuLevelName);
	}
}

void UMatrixGameInstance::AdvanceToNextStreamingLevel()
{
	// TODO: 레벨 스트리밍을 통해 다음 레벨(e.g. 2층)을 로드하는 로직 구현
	UE_LOG(LogTemp, Warning, TEXT("Advancing to next streaming level..."));
}