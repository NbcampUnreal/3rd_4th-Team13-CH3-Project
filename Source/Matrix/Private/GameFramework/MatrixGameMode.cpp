#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "GameFramework/MatrixLevelManager.h"
#include "AI/EnemyCharacter.h"
#include "Core/MatrixCoreTypes.h"
#include "Systems/MatrixSpawnManager.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/MatrixGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MainPlayerController.h"
#include "Characters/MainPlayerCharacter.h"

AMatrixGameMode::AMatrixGameMode()
{
    GameStateClass = AMatrixGameState::StaticClass();
    PlayerControllerClass = AMainPlayerController::StaticClass();
    DefaultPawnClass = AMainPlayerCharacter::StaticClass();
}

void AMatrixGameMode::BeginPlay()
{
    Super::BeginPlay();

    MatrixGameState = GetGameState<AMatrixGameState>();
    
    if (MatrixGameState)
    {
		FString CurrentLevelName = GetWorld()->GetMapName();
		CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

        if(CurrentLevelName == TEXT("L_MainMenu"))
        {
			MatrixGameState->SetGameState(EGameState::MainMenu);
        }
        else
        {
			MatrixGameState->SetGameState(EGameState::Playing);
        }
    }

    if (MatrixGameState && MatrixGameState->CurrentGameState == EGameState::Playing)   
    {
        if (bApplyWhiteMaterialOnStart)
        {
            UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
            if (GameInstance)
            {
                GameInstance->ApplyWhiteMaterialToAllMeshes();
                GameInstance->ApplyWhiteMaterialWithDelay(3.0f);
                GameInstance->StartRepeatingMaterialApplication(5.0f, 8);
                
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("BP_Room"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("BP_Spline_Chairs"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("SM_Rect_Desk"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("BP_Item"));
                
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Column"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Pillar"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Wall"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Building"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Floor"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Ceiling"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Foundation"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Structure"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("Architecture"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("SM_"));
                GameInstance->ApplyMaterialToActorsByNamePattern(TEXT("BP_"));
                
                GameInstance->DebugPrintAllActors();
            }
        }

        if (WaveDataTable)
        {
            CurrentWaveDataTable = WaveDataTable;
            UE_LOG(LogTemp, Warning, TEXT("Wave data loaded. Total waves: %d"), CurrentWaveDataTable->GetRowNames().Num());
            StartWave();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WaveDataTable is not set!"));
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

    // 자동 웨이브 종료 로직 제거 - 트리거 박스를 통해 수동으로 웨이브 제어
    // if (MatrixGameState->EnemiesRemaining == 0)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Wave %d has no enemies to spawn. Ending wave immediately."), MatrixGameState->CurrentWave);
    //     EndWave();
    // }
}

void AMatrixGameMode::EnemyKilled()
{
    if (!MatrixGameState) return;

    MatrixGameState->EnemiesRemaining--;
	MatrixGameState->KillCount++; // 적 처치 시 킬 카운트 증가
	
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
        {
            MPC->NotifyKillCountChanged(MatrixGameState->KillCount);
        }
	}

    UE_LOG(LogTemp, Warning, TEXT("Enemy killed! %d enemies remaining."), MatrixGameState->EnemiesRemaining);

    // 자동 웨이브 시작 로직 제거 - 트리거 박스를 통해 수동으로 웨이브 시작
    // if (MatrixGameState->EnemiesRemaining <= 0)
    // {
    //     EndWave();
    // }
}

void AMatrixGameMode::EndWave()
{
    if (!MatrixGameState) return;

    UE_LOG(LogTemp, Warning, TEXT("Wave %d Cleared!"), MatrixGameState->CurrentWave);

    // 자동 다음 웨이브 시작 로직 제거 - 트리거 박스를 통해 수동으로 웨이브 시작
    // if (MatrixGameState->CurrentWave >= CurrentWaveDataTable->GetRowNames().Num())
    // {
    //     // 모든 웨이브 클리어 - 층별 웨이브 완료 처리
    //     HandleFloorWaveCompletion();
    // }
    // else
    // {
    //     GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMatrixGameMode::StartWave, TimeBetweenWaves);
    // }
}

void AMatrixGameMode::CheckBossStageOrGameClear()
{
    // 보스 스테이지가 있는지 확인
    if (bHasBossStage)
    {
        UE_LOG(LogTemp, Warning, TEXT("All waves cleared! Proceeding to Boss Stage!"));
        // TODO: 보스 스테이지로 전환
        // UGameplayStatics::OpenLevel(this, BossStageLevelName);
        // 또는 보스 스테이지 시작 로직
        StartBossStage();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("All waves cleared! Game Clear!"));
        MatrixGameState->SetGameState(EGameState::GameClear);
    }
}

void AMatrixGameMode::StartBossStage()
{
    // 보스 스테이지 시작 로직
    UE_LOG(LogTemp, Warning, TEXT("Boss Stage Started!"));
    // TODO: 보스 스테이지 관련 로직 구현
    // 예: 보스 스폰, 보스 전용 UI, 보스 전용 게임 규칙 등
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

// === 웨이브 관리 함수들 ===

void AMatrixGameMode::StartSpecificWave(int32 WaveNumber)
{
    if (!MatrixGameState || !CurrentWaveDataTable || !SpawnManager) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start specific wave: Missing required components"));
        return;
    }
    
    // 웨이브 번호 설정
    MatrixGameState->CurrentWave = WaveNumber;
    MatrixGameState->EnemiesRemaining = 0;
    
    FString ContextString;
    FWaveData* CurrentWaveData = CurrentWaveDataTable->FindRow<FWaveData>(FName(*FString::FromInt(WaveNumber)), ContextString);
    
    if (!CurrentWaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave %d data not found"), WaveNumber);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Starting Specific Wave %d!"), WaveNumber);
    
    // 웨이브 데이터에 따라 적 스폰
    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        TArray<AActor*> AvailableSpawnPoints = SpawnManager->GetSpawnPointsForTag(SpawnInfo.SpawnPointTag);
        
        if (AvailableSpawnPoints.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("No spawn points found for tag: %s"), *SpawnInfo.SpawnPointTag.ToString());
            continue;
        }
        
        if (!SpawnInfo.EnemyClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("EnemyClass is not set for spawn command with tag: %s"), *SpawnInfo.SpawnPointTag.ToString());
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
    
    if (MatrixGameState->EnemiesRemaining == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave %d has no enemies to spawn"), WaveNumber);
    }
}

void AMatrixGameMode::ForceStartWave()
{
    if (!MatrixGameState) return;
    
    // 다음 웨이브 시작
    StartWave();
}

void AMatrixGameMode::InitializeLevelStreaming()
{
    UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>();
    if (!LevelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager not found for level streaming initialization"));
        return;
    }
    
    // 모든 서브레벨(1층, 2층, Environment)이 이미 로드되어 있음
    UE_LOG(LogTemp, Log, TEXT("All sublevels (Floor 1, Floor 2, Environment) are already loaded"));
    UE_LOG(LogTemp, Log, TEXT("Floor 2 blocking volumes will be disabled when waves are completed"));
}

void AMatrixGameMode::HandleFloorWaveCompletion()
{
    // 레벨 매니저 가져오기
    UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>();
    if (!LevelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager not found!"));
        return;
    }
    
    int32 CurrentFloor = LevelManager->GetCurrentFloor();
    UE_LOG(LogTemp, Log, TEXT("Floor %d waves completed!"), CurrentFloor);
    
    // 현재 층의 웨이브 완료 상태 설정
    LevelManager->SetFloorWaveCompleted(CurrentFloor, true);
    
    // 2층이면 1층으로 진행
    if (CurrentFloor == 2)
    {
        // 2층 웨이브 완료 - 1층으로 자동 진행
        UE_LOG(LogTemp, Log, TEXT("Floor 2 (2층) waves completed. Automatically proceeding to Floor 1 (1층)."));
        
        // 2층 Blocking Volume 비활성화
        LevelManager->DisableFloorBlockingVolumes(2);
        UE_LOG(LogTemp, Log, TEXT("Disabled Floor 2 (2층) blocking volumes"));
        
        LevelManager->ProceedToNextFloor();
        
        // 1층 웨이브 시작
        if (LevelManager->GetCurrentFloor() == 1)
        {
            UE_LOG(LogTemp, Log, TEXT("Starting Floor 1 (1층) waves..."));
            // 웨이브 카운터 리셋
            MatrixGameState->CurrentWave = 0;
            StartWave(); // 1층 첫 웨이브 시작
        }
    }
    // 1층(Floor 1)이면 게임 클리어
    else if (CurrentFloor == 1)
    {
        // 1층 웨이브 완료 - 게임 클리어 조건 체크
        if (LevelManager->CheckGameClearConditions())
        {
            UE_LOG(LogTemp, Log, TEXT("Game Clear conditions met!"));
            CheckBossStageOrGameClear();
        }
    }
}
