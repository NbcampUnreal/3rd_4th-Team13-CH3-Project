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

