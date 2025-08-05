#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "AI/EnemyCharacter.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/MatrixGameInstance.h"
#include "Kismet/GameplayStatics.h"

AMatrixGameMode::AMatrixGameMode()
{
    GameStateClass = AMatrixGameState::StaticClass();
}

void AMatrixGameMode::BeginPlay()
{
    Super::BeginPlay();

    MatrixGameState = GetGameState<AMatrixGameState>();

    UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
    if (GameInstance && LevelDataTable)
    {
        FString ContextString;
        FLevelData* CurrentLevelData = LevelDataTable->FindRow<FLevelData>(FName(*FString::FromInt(GameInstance->CurrentLevel)), ContextString);

        if (CurrentLevelData)
        {
            CurrentWaveDataTable = CurrentLevelData->WaveDataTable;
            if (CurrentWaveDataTable)
            {
                StartWave();
            }
        }
    }
}

void AMatrixGameMode::StartWave()
{
    if (!MatrixGameState || !CurrentWaveDataTable) return;

    FString ContextString;
    FWaveData* CurrentWaveData = CurrentWaveDataTable->FindRow<FWaveData>(FName(*FString::FromInt(MatrixGameState->CurrentWave + 1)), ContextString);

    if (!CurrentWaveData)
    {
        UE_LOG(LogTemp, Error, TEXT("Wave data not found for wave %d."), MatrixGameState->CurrentWave);
        return;
    }
    
    MatrixGameState->CurrentWave++;
    MatrixGameState->EnemiesRemaining = CurrentWaveData->EnemiesToSpawn;
    
    UE_LOG(LogTemp, Warning, TEXT("Wave %d Started!"), MatrixGameState->CurrentWave);

    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        for (ATargetPoint* SpawnPoint : SpawnInfo.SpawnPoints)
        {
            if (SpawnInfo.EnemyClass && SpawnPoint)
            {
                GetWorld()->SpawnActor<AEnemyCharacter>(SpawnInfo.EnemyClass, SpawnPoint->GetActorTransform());
            }
        }
    }
    
    // FString SpawnPointNamePrefix = FString::Printf(TEXT("SpawnPoint_Wave%d"), MatrixGameState->CurrentWave);
    //
    // TArray<AActor*> SpawnPoints;
    // UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);
    //
    // TArray<AActor*> SpawnPointArray;
    // for (AActor* Actor : SpawnPoints)
    // {
    //     if (Actor->GetActorLabel().StartsWith(SpawnPointNamePrefix))
    //     {
    //         SpawnPointArray.Add(Actor);
    //     }
    // }
    //
    // int32 NumEnemiesToSpawn = SpawnPointArray.Num();
    // if (NumEnemiesToSpawn <= 0)
    // {
    //     UE_LOG(LogTemp, Error, TEXT("No spawn points found for wave %d."), MatrixGameState->CurrentWave);
    // }
    //
    // MatrixGameState->EnemiesRemaining = NumEnemiesToSpawn;
    //
    // for (int32 i = 0; i < NumEnemiesToSpawn; i++)
    // {
    //     if (EnemyToSpawnClass && SpawnPointArray.IsValidIndex(i))
    //     {
    //         AActor* SpawnPoint = SpawnPointArray[i];
    //         GetWorld()->SpawnActor<AEnemyCharacter>(EnemyToSpawnClass, SpawnPoint->GetActorTransform());
    //     }
    // }
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

    if (MatrixGameState->CurrentWave >= CurrentWaveDataTable->GetRowNames().Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Level Cleared! Proceeding to next level."));
        StartNextLevel();
    }
    else
    {
        GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMatrixGameMode::StartWave, TimeBetweenWaves);
    }
}

void AMatrixGameMode::StartNextLevel()
{
    UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->AdvanceToNextLevel();
        // TODO: 실제 레벨 전환 로직 구현
        // UGameplayStatics::OpenLevel
        // 일단 로그만 먼저 출력 후 추후 구현 예정
        // UGameplayStatics::OpenLevel(this, FName("Level_BossFight"));
    }
}