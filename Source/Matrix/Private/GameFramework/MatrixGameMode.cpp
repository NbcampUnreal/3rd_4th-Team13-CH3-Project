#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "AI/EnemyCharacter.h"
#include "Core/MatrixSpawnManager.h"
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

    SpawnManager = Cast<AMatrixSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMatrixSpawnManager::StaticClass()));
    if (!SpawnManager)
    {
        UE_LOG(LogTemp, Error, TEXT("FATAL ERROR: SpawnManager not found in the level! Spawning will not work."));
    }

    UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
    if (GameInstance && LevelDataTable)
    {
        FString ContextString;
        FLevelData* CurrentLevelData = LevelDataTable->FindRow<FLevelData>(FName(*FString::FromInt(GameInstance->CurrentLevel)), ContextString);
        
        if (CurrentLevelData)
        {
            UE_LOG(LogTemp, Warning, TEXT("Level %d"), GameInstance->CurrentLevel);
            CurrentWaveDataTable = CurrentLevelData->WaveDataTable;
            if (CurrentWaveDataTable)
            {
                UE_LOG(LogTemp, Warning, TEXT("Level %d has %d waves."), GameInstance->CurrentLevel, CurrentWaveDataTable->GetRowNames().Num());
                StartWave();
            }
        }
    }
}

void AMatrixGameMode::StartWave()
{
    if (!MatrixGameState || !CurrentWaveDataTable || !SpawnManager) return;

    MatrixGameState->CurrentWave++;
    MatrixGameState->EnemiesRemaining = 0; // 웨이브 시작 시 스폰할 적 수를 0으로 초기화

    FString ContextString;
    FWaveData* CurrentWaveData = CurrentWaveDataTable->FindRow<FWaveData>(FName(*FString::FromInt(MatrixGameState->CurrentWave)), ContextString);

    if (!CurrentWaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave %d data not found. Assuming all waves for this level are complete."), MatrixGameState->CurrentWave);
        EndWave(); // 다음 웨이브 데이터가 없으면 즉시 웨이브 종료 처리
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Wave %d Started!"), MatrixGameState->CurrentWave);

    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        TArray<AActor*> AvailableSpawnPoints = SpawnManager->GetSpawnPointsForTag(SpawnInfo.SpawnPointTag);

        if (AvailableSpawnPoints.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("No spawn points found for tag: %s. Skipping this spawn command."), *SpawnInfo.SpawnPointTag.ToString());
            continue;
        }

        if (!SpawnInfo.EnemyClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("EnemyClass is not set for spawn command with tag: %s."), *SpawnInfo.SpawnPointTag.ToString());
            continue;
        }

        MatrixGameState->EnemiesRemaining += SpawnInfo.SpawnCount;

        for (int32 i = 0; i < SpawnInfo.SpawnCount; ++i)
        {
            AActor* SpawnPoint = AvailableSpawnPoints[i % AvailableSpawnPoints.Num()];

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            GetWorld()->SpawnActor<AEnemyCharacter>(SpawnInfo.EnemyClass, SpawnPoint->GetActorTransform(), SpawnParams);
        }
    }

    // 만약 웨이브에 스폰할 적이 하나도 없었다면, 즉시 웨이브 종료 처리
    if (MatrixGameState->EnemiesRemaining == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave %d has no enemies to spawn. Ending wave immediately."), MatrixGameState->CurrentWave);
        EndWave();
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