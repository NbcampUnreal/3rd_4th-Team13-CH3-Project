#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "AI/EnemyCharacter.h"
#include "Core/MatrixGameTypes.h"
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
    if (MatrixGameState)
    {
        MatrixGameState->SetGameState(EGameState::Playing);
    }

    if (MatrixGameState && MatrixGameState->CurrentGameState == EGameState::Playing)
    {
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
        PrepareNextLevel();
    }
    else
    {
        GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMatrixGameMode::StartWave, TimeBetweenWaves);
    }
}

void AMatrixGameMode::PrepareNextLevel()
{
    UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->AdvanceToNextLevel();
        CheckGameClearCondition();
    }
}

void AMatrixGameMode::CheckGameClearCondition()
{
    UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
    if (GameInstance && MatrixGameState)
    {
        if (GameInstance->CurrentLevel > GameInstance->MaxLevel)
        {
            UE_LOG(LogTemp, Warning, TEXT("모든 레벨 클리어! Game Clear!"))
            MatrixGameState->SetGameState(EGameState::GameClear);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Proceeding to Level %d"), GameInstance->CurrentLevel);
            GameInstance->AdvanceToNextStreamingLevel();
            // TOdo: 새 레벨의 웨이브 데이터 테이블 로드 & StartWave()
        }
    }
}

void AMatrixGameMode::PlayerDied()
{
    if (MatrixGameState)
    {
        MatrixGameState->SetGameState(EGameState::GameOver);
    }
}

void AMatrixGameMode::RequestTogglePause()
{
    if (MatrixGameState)
    {
        EGameState CurrentState = MatrixGameState->CurrentGameState;
        // 오직 '플레이 중'일 때만 '일시정지'로 OR '일시정지' 상태일 때만 '플레이 중'으로 변경 가능
        if (CurrentState == EGameState::Playing)
        {
            MatrixGameState->SetGameState(EGameState::Paused);
            UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 월드 시간 정지
        }
        else if (CurrentState == EGameState::Paused)
        {
            MatrixGameState->SetGameState(EGameState::Playing);
            UGameplayStatics::SetGamePaused(GetWorld(), false); // 게임 월드 시간 재개
        }
        // GameOver나 GameClear 상태에서는 일시정지 불가!!
    }
}

void AMatrixGameMode::RegisterSpawnManager(AMatrixSpawnManager* InSpawnManager)
{
    SpawnManager = InSpawnManager;
    UE_LOG(LogTemp, Warning, TEXT("SpawnManager has been registered to GameMode!"));
}
