// MatrixGameMode.cpp

#include "MatrixGameMode.h"
#include "MatrixGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void AMatrixGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartWave();
}

void AMatrixGameMode::StartWave()
{
	AMatrixGameState* GameState = GetGameState<AMatrixGameState>();
	if (GameState)
	{
		GameState->CurrentWave++;
		// MVP에서는 웨이브마다 2명의 적을 스폰한다고 가정
		NumEnemiesToSpawn = 2; 
		GameState->EnemiesRemaining = NumEnemiesToSpawn;

		UE_LOG(LogTemp, Warning, TEXT("Wave %d Started! Spawning %d enemies."), GameState->CurrentWave, NumEnemiesToSpawn);

		// TODO: 여기에 실제 적 스폰 로직을 구현
		// MVP 단계에서는 스폰 지점을 하드코딩하거나 간단한 로직으로 배치할 예정.
	}
}

void AMatrixGameMode::EnemyKilled()
{
	AMatrixGameState* GameState = GetGameState<AMatrixGameState>();
	if (GameState)
	{
		GameState->EnemiesRemaining--;
		UE_LOG(LogTemp, Warning, TEXT("Enemy killed! %d enemies remaining."), GameState->EnemiesRemaining);

		if (GameState->EnemiesRemaining <= 0)
		{
			EndWave();
		}
	}
}

void AMatrixGameMode::EndWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Cleared!"), GetGameState<AMatrixGameState>()->CurrentWave);

	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMatrixGameMode::StartWave, TimeBetweenWaves);
}