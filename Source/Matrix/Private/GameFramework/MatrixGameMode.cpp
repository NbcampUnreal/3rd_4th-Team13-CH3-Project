#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "AI/EnemyCharacter.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

AMatrixGameMode::AMatrixGameMode()
{
    GameStateClass = AMatrixGameState::StaticClass();
}

void AMatrixGameMode::BeginPlay()
{
    Super::BeginPlay();

    MatrixGameState = GetGameState<AMatrixGameState>();
    
    StartWave();
}

void AMatrixGameMode::StartWave()
{
    if (!MatrixGameState) return;

    MatrixGameState->CurrentWave++;
    UE_LOG(LogTemp, Warning, TEXT("Wave %d Started!"), MatrixGameState->CurrentWave);
    
    FString SpawnPointNamePrefix = FString::Printf(TEXT("SpawnPoint_Wave%d"), MatrixGameState->CurrentWave);

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
        UE_LOG(LogTemp, Error, TEXT("No spawn points found for wave %d."), MatrixGameState->CurrentWave);
    }
    
    MatrixGameState->EnemiesRemaining = NumEnemiesToSpawn;

    for (int32 i = 0; i < NumEnemiesToSpawn; i++)
    {
        if (EnemyToSpawnClass && SpawnPointArray.IsValidIndex(i))
        {
            AActor* SpawnPoint = SpawnPointArray[i];
            GetWorld()->SpawnActor<AEnemyCharacter>(EnemyToSpawnClass, SpawnPoint->GetActorTransform());
        }
    }
}

void AMatrixGameMode::EnemyKilled()
{
    if (!MatrixGameState) return;

    MatrixGameState->EnemiesRemaining--;
    UE_LOG(LogTemp, Warning, TEXT("Enemy killed! %d enemies remaining."), MatrixGameState->EnemiesRemaining);

    if (MatrixGameState->EnemiesRemaining <= 0)
    {
        EndWave();
    }
}

void AMatrixGameMode::EndWave()
{
    if (!MatrixGameState) return;

    UE_LOG(LogTemp, Warning, TEXT("Wave %d Cleared!"), MatrixGameState->CurrentWave);
    
    GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMatrixGameMode::StartWave, TimeBetweenWaves);
}