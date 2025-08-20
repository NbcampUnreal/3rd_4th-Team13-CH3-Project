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
            MatrixGameState->SetGameState(GameInstance->GetPersistentGameState());
        }
        else
        {
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
                GameInstance->ApplyWhiteMaterialToAllMeshes();
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
        return;
    }
    
    // 게임이 플레이 중 상태가 아니면 웨이브 시작하지 않음
    if (MatrixGameState->CurrentGameState != EGameState::Playing)
    {
        return;
    }

    MatrixGameState->CurrentWave++;
    MatrixGameState->EnemiesRemaining = 0; // 웨이브 시작 시 스폰할 적 수를 0으로 초기화

    FString ContextString;
    FWaveData* CurrentWaveData = CurrentWaveDataTable->FindRow<FWaveData>(FName(*FString::FromInt(MatrixGameState->CurrentWave)), ContextString);

    if (!CurrentWaveData)
    {
        EndWave(); // 다음 웨이브 데이터가 없으면 즉시 웨이브 종료 처리
        return;
    }

    // 웨이브 전체에서 사용할 모든 스폰 포인트를 수집하고 셔플
    TSet<AActor*> WaveSpawnPoints;
    
    // 모든 SpawnInfo에서 사용할 스폰 포인트들을 수집 (중복 제거)
    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        TArray<AActor*> AvailableSpawnPoints = SpawnManager->GetSpawnPointsForTag(SpawnInfo.SpawnPointTag);
        for (AActor* Point : AvailableSpawnPoints)
        {
            WaveSpawnPoints.Add(Point);
        }
    }
    
    // 웨이브 스폰 포인트를 배열로 변환하고 셔플
    TArray<AActor*> ShuffledSpawnPoints = WaveSpawnPoints.Array();
    for (int32 i = ShuffledSpawnPoints.Num() - 1; i > 0; --i)
    {
        int32 RandomIndex = FMath::RandRange(0, i);
        ShuffledSpawnPoints.Swap(i, RandomIndex);
    }
    
    // 웨이브 전체에서 순차적으로 사용할 인덱스
    int32 GlobalSpawnPointIndex = 0;

    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        if (!SpawnInfo.EnemyClass)
        {
            continue;
        }

        MatrixGameState->EnemiesRemaining += SpawnInfo.SpawnCount;

        for (int32 i = 0; i < SpawnInfo.SpawnCount; ++i)
        {
            AActor* SpawnPoint = nullptr;
            
            // 셔플된 스폰 포인트 배열에서 순차적으로 선택
            if (GlobalSpawnPointIndex < ShuffledSpawnPoints.Num())
            {
                SpawnPoint = ShuffledSpawnPoints[GlobalSpawnPointIndex];
                GlobalSpawnPointIndex++;
            }
            else
            {
                // 모든 스폰 포인트를 사용했으면 처음부터 다시 시작
                GlobalSpawnPointIndex = 0;
                if (ShuffledSpawnPoints.Num() > 0)
                {
                    SpawnPoint = ShuffledSpawnPoints[GlobalSpawnPointIndex];
                    GlobalSpawnPointIndex++;
                }
            }

            if (!SpawnPoint)
            {
                UE_LOG(LogTemp, Error, TEXT("No spawn point available!"));
                continue;
            }

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(SpawnInfo.EnemyClass, SpawnPoint->GetActorTransform(), SpawnParams);
            
            // 스폰된 적에게 무기 유형 지정
            if (SpawnedEnemy)
            {
                if (SpawnInfo.WeaponType != EWeaponType::None)
                {
                    SpawnedEnemy->ChangeWeapon(SpawnInfo.WeaponType);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy!"));
            }
        }
    }
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



    // 모든 적을 처치했을 때 게임 클리어 조건 체크
    if (MatrixGameState->EnemiesRemaining <= 0)
    {
        CheckGameClearConditions();
    }
}

void AMatrixGameMode::EndWave()
{
    if (!MatrixGameState) return;
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
    
    // 킬 카운트가 총 스폰할 적 수와 같거나 크면 게임 클리어
    if (MatrixGameState->KillCount >= TotalEnemiesToSpawn)
    {
        CheckBossStageOrGameClear();
    }
}

void AMatrixGameMode::CheckBossStageOrGameClear()
{
    // 보스 스테이지가 있는지 확인
    if (bHasBossStage)
    {
        StartBossStage();
    }
    else
    {
        // GameInstance를 통한 게임 클리어 처리
        if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
        {
            GameInstance->SetPersistentGameState(EGameState::GameClear);
        }
        else
        {
            if (MatrixGameState)
            {
                MatrixGameState->SetGameState(EGameState::GameClear);
            }
        }
    }
}

void AMatrixGameMode::StartBossStage()
{
    // TODO: 보스 스테이지 관련 로직 구현
}

void AMatrixGameMode::PlayerDied()
{
    // GameInstance를 통한 게임 오버 처리
    if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
    {
        GameInstance->SetPersistentGameState(EGameState::GameOver);
    }
    else
    {
        if (MatrixGameState)
        {
            MatrixGameState->SetGameState(EGameState::GameOver);
        }
    }
}

void AMatrixGameMode::RequestTogglePause()
{
    // GameInstance를 통한 일시정지 처리
    if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
    {
        EGameState CurrentState = GameInstance->GetPersistentGameState();
        
        // 오직 '플레이 중'일 때만 '일시정지'로 OR '일시정지' 상태일 때만 '플레이 중'으로 변경 가능
        if (CurrentState == EGameState::Playing)
        {
            UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: Pausing game via GameInstance"));
            GameInstance->SetPersistentGameState(EGameState::Paused);
            UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 월드 시간 정지
        }
        else if (CurrentState == EGameState::Paused)
        {
            UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: Resuming game via GameInstance"));
            GameInstance->SetPersistentGameState(EGameState::Playing);
            UGameplayStatics::SetGamePaused(GetWorld(), false); // 게임 월드 시간 재개
        }
        // GameOver나 GameClear 상태에서는 일시정지 불가!!
    }
    else
    {
        // 백업: 기존 방식 사용
        UE_LOG(LogTemp, Warning, TEXT("MatrixGameMode: GameInstance not found, using fallback method"));
        if (MatrixGameState)
        {
            EGameState CurrentState = MatrixGameState->CurrentGameState;
            if (CurrentState == EGameState::Playing)
            {
                MatrixGameState->SetGameState(EGameState::Paused);
                UGameplayStatics::SetGamePaused(GetWorld(), true);
            }
            else if (CurrentState == EGameState::Paused)
            {
                MatrixGameState->SetGameState(EGameState::Playing);
                UGameplayStatics::SetGamePaused(GetWorld(), false);
            }
        }
    }
}

void AMatrixGameMode::RegisterSpawnManager(AMatrixSpawnManager* InSpawnManager)
{
    SpawnManager = InSpawnManager;
}

// === 웨이브 관리 함수들 ===

void AMatrixGameMode::StartSpecificWave(int32 WaveNumber)
{
    if (!MatrixGameState || !CurrentWaveDataTable || !SpawnManager) 
    {
        return;
    }
    
    // 웨이브 번호 설정
    MatrixGameState->CurrentWave = WaveNumber;
    MatrixGameState->EnemiesRemaining = 0;
    
    FString ContextString;
    FWaveData* CurrentWaveData = CurrentWaveDataTable->FindRow<FWaveData>(FName(*FString::FromInt(WaveNumber)), ContextString);
    
    if (!CurrentWaveData)
    {
        return;
    }
    
    // 웨이브 전체에서 사용할 모든 스폰 포인트를 수집하고 셔플
    TSet<AActor*> WaveSpawnPoints;
    
    // 모든 SpawnInfo에서 사용할 스폰 포인트들을 수집 (중복 제거)
    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        TArray<AActor*> AvailableSpawnPoints = SpawnManager->GetSpawnPointsForTag(SpawnInfo.SpawnPointTag);
        for (AActor* Point : AvailableSpawnPoints)
        {
            WaveSpawnPoints.Add(Point);
        }
    }
    
    // 웨이브 스폰 포인트를 배열로 변환하고 셔플
    TArray<AActor*> ShuffledSpawnPoints = WaveSpawnPoints.Array();
    for (int32 i = ShuffledSpawnPoints.Num() - 1; i > 0; --i)
    {
        int32 RandomIndex = FMath::RandRange(0, i);
        ShuffledSpawnPoints.Swap(i, RandomIndex);
    }
    
    // 웨이브 전체에서 순차적으로 사용할 인덱스
    int32 GlobalSpawnPointIndex = 0;
    
    // 웨이브 데이터에 따라 적 스폰
    for (const FEnemySpawnInfo& SpawnInfo : CurrentWaveData->SpawnInfos)
    {
        if (!SpawnInfo.EnemyClass)
        {
            continue;
        }
        
        MatrixGameState->EnemiesRemaining += SpawnInfo.SpawnCount;
        
        for (int32 i = 0; i < SpawnInfo.SpawnCount; ++i)
        {
            AActor* SpawnPoint = nullptr;
            
            // 셔플된 스폰 포인트 배열에서 순차적으로 선택
            if (GlobalSpawnPointIndex < ShuffledSpawnPoints.Num())
            {
                SpawnPoint = ShuffledSpawnPoints[GlobalSpawnPointIndex];
                GlobalSpawnPointIndex++;
            }
            else
            {
                // 모든 스폰 포인트를 사용했으면 처음부터 다시 시작
                GlobalSpawnPointIndex = 0;
                if (ShuffledSpawnPoints.Num() > 0)
                {
                    SpawnPoint = ShuffledSpawnPoints[GlobalSpawnPointIndex];
                    GlobalSpawnPointIndex++;
                }
            }
            
            if (!SpawnPoint)
            {
                UE_LOG(LogTemp, Error, TEXT("No spawn point available!"));
                continue;
            }
            
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(SpawnInfo.EnemyClass, SpawnPoint->GetActorTransform(), SpawnParams);
            
            // 스폰된 적에게 무기 유형 지정
            if (SpawnedEnemy)
            {
                if (SpawnInfo.WeaponType != EWeaponType::None)
                {
                    SpawnedEnemy->ChangeWeapon(SpawnInfo.WeaponType);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy!"));
            }
        }
    }
}

void AMatrixGameMode::ForceStartWave()
{
    if (!MatrixGameState) return;
    
    // 다음 웨이브 시작
    StartWave();
}


