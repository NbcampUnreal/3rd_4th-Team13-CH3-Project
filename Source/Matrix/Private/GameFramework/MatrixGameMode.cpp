#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "AI/EnemyCharacter.h"
#include "Core/MatrixCoreTypes.h"
#include "Core/MatrixWaveTypes.h"
#include "Systems/MatrixSpawnManager.h"
#include "Systems/MatrixSpawnPoint.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/MatrixGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MainPlayerController.h"
#include "Characters/MainPlayerCharacter.h"
#include "GameplayTagAssetInterface.h"
#include "Weapons/WeaponSystem/WeaponBase.h"

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

        // GameInstance의 상태와 동기화
        if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
        {
            UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: Synchronizing with GameInstance state: %d"), 
                (int32)GameInstance->GetPersistentGameState());
            
            // GameInstance의 상태를 사용
            MatrixGameState->SetGameState(GameInstance->GetPersistentGameState());
        }
        else
        {
            // 백업: 기존 방식 사용
            UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: GameInstance not found, using fallback method"));
            if(CurrentLevelName == TEXT("L_MainMenu"))
            {
                MatrixGameState->SetGameState(EGameState::MainMenu);
            }
            else
            {
                MatrixGameState->SetGameState(EGameState::Playing);
            }
        }
    }

    if (MatrixGameState && MatrixGameState->CurrentGameState == EGameState::Playing)   
    {
        if (bApplyWhiteMaterialOnStart)
        {
            UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance());
            if (GameInstance)
            {
                // 맵 로드 시 한 번만 머티리얼 적용
                GameInstance->ApplyWhiteMaterialToAllMeshes();
                
                // 반복 실행 제거 - 한 번만 실행
                // GameInstance->ApplyWhiteMaterialWithDelay(3.0f);
                // GameInstance->StartRepeatingMaterialApplication(5.0f, 8);
                
                // 특정 액터들에 대해서도 한 번만 적용
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
                
                // 디버그 출력도 한 번만
                GameInstance->DebugPrintAllActors();
                
                UE_LOG(LogTemp, Warning, TEXT("White material applied once on map load"));
            }
        }

        if (WaveDataTable)
        {
            CurrentWaveDataTable = WaveDataTable;
            UE_LOG(LogTemp, Warning, TEXT("Wave data loaded. Total waves: %d"), CurrentWaveDataTable->GetRowNames().Num());
            // 자동 웨이브 시작 제거 - 트리거 박스를 통해서만 웨이브 시작
            // StartWave();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WaveDataTable is not set!"));
        }
    }
}

void AMatrixGameMode::StartWave()
{
    if (!MatrixGameState || !CurrentWaveDataTable || !SpawnManager) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start wave: Missing required components"));
        return;
    }
    
    // 게임이 플레이 중 상태가 아니면 웨이브 시작하지 않음
    if (MatrixGameState->CurrentGameState != EGameState::Playing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start wave: Game is not in Playing state"));
        return;
    }

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
            AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(SpawnInfo.EnemyClass, SpawnPoint->GetActorTransform(), SpawnParams);
            
            // 스폰된 적에게 무기 유형 지정
            if (SpawnedEnemy)
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy spawned successfully: %s"), *SpawnedEnemy->GetName());
                UE_LOG(LogTemp, Warning, TEXT("WeaponType from SpawnInfo: %d"), (int32)SpawnInfo.WeaponType);
                
                if (SpawnInfo.WeaponType != EWeaponType::None)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Attempting to change weapon to type: %d"), (int32)SpawnInfo.WeaponType);
                    SpawnedEnemy->ChangeWeapon(SpawnInfo.WeaponType);
                    
                    // 무기 변경 후 확인
                    AWeaponBase* EquippedWeapon = SpawnedEnemy->GetEquippedWeapon();
                    if (EquippedWeapon)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Weapon changed successfully: %s (Type: %d)"), 
                            *EquippedWeapon->GetName(), (int32)EquippedWeapon->GetWeaponType());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Weapon change failed - no weapon equipped"));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("WeaponType is None, using default weapon"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy!"));
            }
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

    // 모든 적을 처치했을 때 게임 클리어 조건 체크
    if (MatrixGameState->EnemiesRemaining <= 0)
    {
        CheckGameClearConditions();
    }
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

void AMatrixGameMode::CheckGameClearConditions()
{
    if (!MatrixGameState || !CurrentWaveDataTable) return;

    // 웨이브 데이터에서 총 스폰할 적 수 계산
    int32 TotalEnemiesToSpawn = 0;
    FString ContextString;
    
    for (const FName& RowName : CurrentWaveDataTable->GetRowNames())
    {
        FWaveData* WaveData = CurrentWaveDataTable->FindRow<FWaveData>(RowName, ContextString);
        if (WaveData)
        {
            for (const FEnemySpawnInfo& SpawnInfo : WaveData->SpawnInfos)
            {
                TotalEnemiesToSpawn += SpawnInfo.SpawnCount;
            }
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Kill Count: %d, Total Enemies to Spawn: %d"), MatrixGameState->KillCount, TotalEnemiesToSpawn);
    
    // 킬 카운트가 총 스폰할 적 수와 같거나 크면 게임 클리어
    if (MatrixGameState->KillCount >= TotalEnemiesToSpawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("All enemies defeated! Checking for game clear..."));
        
        // 새로운 킬 카운트 기반 로직으로 직접 게임 클리어 처리
        // 레벨 매니저의 웨이브 완료 체크와 충돌을 피하기 위해 우회
        CheckBossStageOrGameClear();
    }
    else
    {
        // 아직 모든 적을 처치하지 못한 경우
        UE_LOG(LogTemp, Warning, TEXT("Enemies remaining: %d/%d"), TotalEnemiesToSpawn - MatrixGameState->KillCount, TotalEnemiesToSpawn);
    }
}

void AMatrixGameMode::CheckBossStageOrGameClear()
{
    UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: CheckBossStageOrGameClear called"));
    
    // 보스 스테이지가 있는지 확인
    if (bHasBossStage)
    {
        UE_LOG(LogTemp, Warning, TEXT("All waves cleared! Proceeding to Boss Stage!"));
        StartBossStage();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("All waves cleared! Game Clear!"));
        
        // GameInstance를 통한 게임 클리어 처리
        if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
        {
            UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: Setting game clear state via GameInstance"));
            GameInstance->SetPersistentGameState(EGameState::GameClear);
        }
        else
        {
            // 백업: 기존 방식 사용
            UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: GameInstance not found, using fallback method"));
            if (MatrixGameState)
            {
                MatrixGameState->SetGameState(EGameState::GameClear);
            }
        }
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
    UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: PlayerDied called"));
    
    // GameInstance를 통한 게임 오버 처리
    if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
    {
        UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: Setting game over state via GameInstance"));
        GameInstance->SetPersistentGameState(EGameState::GameOver);
    }
    else
    {
        // 백업: 기존 방식 사용
        UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: GameInstance not found, using fallback method"));
        if (MatrixGameState)
        {
            MatrixGameState->SetGameState(EGameState::GameOver);
        }
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
            AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(SpawnInfo.EnemyClass, SpawnPoint->GetActorTransform(), SpawnParams);
            
            // 스폰된 적에게 무기 유형 지정
            if (SpawnedEnemy)
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy spawned successfully: %s"), *SpawnedEnemy->GetName());
                UE_LOG(LogTemp, Warning, TEXT("WeaponType from SpawnInfo: %d"), (int32)SpawnInfo.WeaponType);
                
                if (SpawnInfo.WeaponType != EWeaponType::None)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Attempting to change weapon to type: %d"), (int32)SpawnInfo.WeaponType);
                    SpawnedEnemy->ChangeWeapon(SpawnInfo.WeaponType);
                    
                    // 무기 변경 후 확인
                    AWeaponBase* EquippedWeapon = SpawnedEnemy->GetEquippedWeapon();
                    if (EquippedWeapon)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Weapon changed successfully: %s (Type: %d)"), 
                            *EquippedWeapon->GetName(), (int32)EquippedWeapon->GetWeaponType());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Weapon change failed - no weapon equipped"));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("WeaponType is None, using default weapon"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy!"));
            }
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


