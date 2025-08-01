#include "GameFramework/MatrixGameState.h"

#include "AI/EnemyCharacter.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

AMatrixGameState::AMatrixGameState()
{
	CurrentWave = 0;
	EnemiesRemaining = 0;
	TimeBetweenWaves = 10.0f;
}

void AMatrixGameState::BeginPlay()
{
	Super::BeginPlay();
	StartWave();
}

void AMatrixGameState::StartWave()
{
	CurrentWave++;
	// MVP에서는 웨이브마다 2명의 적을 스폰한다고 가정

	UE_LOG(LogTemp, Warning, TEXT("Wave %d Started!"), CurrentWave);

	FString SpawnPointNamePrefix = FString::Printf(TEXT("SpawnPoint_Wave%d"), CurrentWave);

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);

	TArray<AActor*> SpawnPointArray;
	for (AActor* Actor : SpawnPoints)
	{
		if (Actor->GetActorLabel().StartsWith(SpawnPointNamePrefix))
		{
			SpawnPointArray.Add(Actor);
		}
	}

	int32 NumEnemiesToSpawn = SpawnPointArray.Num();
	if (NumEnemiesToSpawn <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No spawn points found with name prefix %s"), *SpawnPointNamePrefix);
	}
	
	for (int32 i = 0; i < NumEnemiesToSpawn; i++)
	{
		AActor* SpawnPoint = SpawnPointArray[i % SpawnPointArray.Num()];

		if (EnemyToSpawnClass)
		{
			GetWorld()->SpawnActor<AEnemyCharacter>(EnemyToSpawnClass, SpawnPoint->GetActorTransform());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EnemyToSpawnClass를 MatrixGameState 블루프린트에 셋팅해주세요."));
		}
	}
}

void AMatrixGameState::EnemyKilled()
{
	EnemiesRemaining--;
	UE_LOG(LogTemp, Warning, TEXT("Enemy killed! %d enemies remaining."), EnemiesRemaining);

	if (EnemiesRemaining <= 0)
	{
		EndWave();
	}
}

void AMatrixGameState::EndWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Cleared!"), CurrentWave);

	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMatrixGameState::StartWave, TimeBetweenWaves);
}