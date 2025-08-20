#include "GameFramework/MatrixLevelManager.h"
#include "GameFramework/MatrixGameState.h"
#include "GameFramework/MatrixGameMode.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/LevelStreaming.h"

UMatrixLevelManager::UMatrixLevelManager()
{
    // 기본값 설정
    CurrentLevel = NAME_None;
    bIsInGame = false;
    bIsTransitioning = false;
    GameStartTime = 0.0f;
    
    // 층별 진행 관리 초기화
    CurrentFloor = 2; // 3층에서 시작
    FloorWaveCompletionStatus.Empty();
    
    // 층별 서브레벨 이름 설정
    FloorSubLevelNames.Add(2, TEXT("S_Floor2"));
    FloorSubLevelNames.Add(1, TEXT("S_Floor1"));
}

void UMatrixLevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // 현재 레벨 설정
    if (UWorld* World = GetWorld())
    {
        CurrentLevel = FName(*World->GetMapName());
    }
}

void UMatrixLevelManager::Deinitialize()
{
    Super::Deinitialize();
}

// === 게임 시작/끝 관리 ===

void UMatrixLevelManager::StartGame()
{
    if (bIsInGame)
    {
        return;
    }
    
    // 게임 상태 초기화
    InitializeGameState();
    
    // 게임 시작 레벨로 전환
    RequestLevelTransition(GameStartLevel, ELevelTransitionType::GameStart);

    if (AMatrixGameState* MartixGameState = GetWorld()->GetGameState<AMatrixGameState>())
    {
		MartixGameState->SetGameState(EGameState::Playing);
    }
}

void UMatrixLevelManager::EndGame(const FString& EndReason)
{
    if (!bIsInGame)
    {
        return;
    }
    
    // 게임 종료 이벤트 브로드캐스트
    OnGameEnded.Broadcast(EndReason);
    
    // 게임 상태 정리
    CleanupGameState();
    
    // 메인 메뉴로 돌아가기
    GoToMainMenu();
}

void UMatrixLevelManager::GoToMainMenu()
{
    RequestLevelTransition(MainMenuLevel, ELevelTransitionType::MainMenu);
}

void UMatrixLevelManager::RestartGame()
{
    // 게임 상태 정리
    CleanupGameState();
    
    // 게임 재시작
    StartGame();
}

// === 레벨 전환 관리 ===

void UMatrixLevelManager::RequestLevelTransition(const FName& TargetLevel, ELevelTransitionType TransitionType)
{
    if (bIsTransitioning)
    {
        UE_LOG(LogTemp, Warning, TEXT("Level transition already in progress"));
        return;
    }
    
    if (TargetLevel == CurrentLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already in target level: %s"), *TargetLevel.ToString());
        return;
    }
    

    
    // 레벨 전환 시작 이벤트 브로드캐스트
    OnLevelTransitionStarted.Broadcast(TargetLevel);
    
    // 실제 레벨 전환 실행
    ExecuteLevelTransition(TargetLevel, TransitionType);
}

void UMatrixLevelManager::ExecuteLevelTransition(const FName& TargetLevel, ELevelTransitionType TransitionType)
{
    bIsTransitioning = true;
    
    // 레벨 전환 타입에 따른 처리
    switch (TransitionType)
    {
    case ELevelTransitionType::MainMenu:
        // 메인 메뉴로 전환
        UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
        break;
        
    case ELevelTransitionType::GameStart:
        // 게임 시작
        UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
        break;
        
    case ELevelTransitionType::GameOver:
        // 게임 오버 화면으로
        UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
        break;
        
    case ELevelTransitionType::GameClear:
        // 게임 클리어 화면으로
        UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
        break;
        
    case ELevelTransitionType::NextLevel:
        // 다음 레벨로
        UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
        break;
        
    case ELevelTransitionType::Restart:
        // 현재 레벨 재시작
        UGameplayStatics::OpenLevel(GetWorld(), CurrentLevel);
        break;
        
    default:
        // 기본 전환
        UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
        break;
    }
    
    // 레벨 전환 완료 처리 (다음 프레임에서)
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, TargetLevel]()
    {
        OnLevelTransitionFinished();
        CurrentLevel = TargetLevel;
        OnLevelTransitionCompleted.Broadcast(TargetLevel);
    }, 0.1f, false);
}

void UMatrixLevelManager::OnLevelTransitionFinished()
{
    bIsTransitioning = false;
}

// === 서브레벨 스트리밍 함수들 ===

void UMatrixLevelManager::LoadSubLevel(const FName& SubLevelName, bool bMakeVisibleAfterLoad)
{
    if (SubLevelName.IsNone())
    {
        return;
    }
    
    if (IsSubLevelLoaded(SubLevelName))
    {
        return;
    }
    
    // 서브레벨 로드
    UGameplayStatics::LoadStreamLevel(GetWorld(), SubLevelName, true, bMakeVisibleAfterLoad, 
        FLatentActionInfo(0, 0, TEXT("OnSubLevelLoaded"), this));
}

void UMatrixLevelManager::UnloadSubLevel(const FName& SubLevelName)
{
    if (SubLevelName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("SubLevel name is None"));
        return;
    }
    
    if (!IsSubLevelLoaded(SubLevelName))
    {
        UE_LOG(LogTemp, Warning, TEXT("SubLevel %s is not loaded"), *SubLevelName.ToString());
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Unloading sublevel: %s"), *SubLevelName.ToString());
    
    // 서브레벨 언로드
    UGameplayStatics::UnloadStreamLevel(GetWorld(), SubLevelName, 
        FLatentActionInfo(0, 0, TEXT("OnSubLevelUnloaded"), this), false);
}

bool UMatrixLevelManager::IsSubLevelLoaded(const FName& SubLevelName) const
{
    if (SubLevelName.IsNone()) return false;
    
    UWorld* World = GetWorld();
    if (!World) return false;
    
    // 현재 로드된 서브레벨들 확인
    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel && StreamingLevel->GetWorldAssetPackageFName() == SubLevelName)
        {
            return StreamingLevel->IsLevelLoaded();
        }
    }
    
    return false;
}

void UMatrixLevelManager::OnSubLevelLoaded(const FName& SubLevelName)
{
    UE_LOG(LogTemp, Log, TEXT("SubLevel %s loaded successfully"), *SubLevelName.ToString());
}

void UMatrixLevelManager::OnSubLevelUnloaded(const FName& SubLevelName)
{
    UE_LOG(LogTemp, Log, TEXT("SubLevel %s unloaded successfully"), *SubLevelName.ToString());
}

FName UMatrixLevelManager::GetCurrentLevel() const
{
    return CurrentLevel;
}

bool UMatrixLevelManager::IsInGame() const
{
    return bIsInGame;
}

// === 내부 함수들 ===

void UMatrixLevelManager::InitializeGameState()
{
    bIsInGame = true;
    GameStartTime = GetWorld()->GetTimeSeconds();
    
    UE_LOG(LogTemp, Log, TEXT("Game state initialized. Start time: %f"), GameStartTime);
}

void UMatrixLevelManager::CleanupGameState()
{
	bIsInGame = false;
	GameStartTime = 0.0f;
	
	UE_LOG(LogTemp, Log, TEXT("Game state cleaned up"));
}

// === 레벨 정보 관리 함수들 ===

FLevelInfo UMatrixLevelManager::GetLevelInfo(const FName& LevelName) const
{
	for (const FLevelInfo& LevelInfo : LevelInfos)
	{
		if (LevelInfo.LevelName == LevelName)
		{
			return LevelInfo;
		}
	}
	
	// 기본 레벨 정보 반환
	FLevelInfo DefaultInfo;
	DefaultInfo.LevelName = LevelName;
	DefaultInfo.LevelType = ELevelType::Gameplay;
	DefaultInfo.DisplayName = LevelName.ToString();
	DefaultInfo.bIsUnlocked = true;
	DefaultInfo.ProgressState = ELevelProgressState::NotStarted;
	
	return DefaultInfo;
}

ELevelType UMatrixLevelManager::GetCurrentLevelType() const
{
	return GetLevelInfo(CurrentLevel).LevelType;
}

bool UMatrixLevelManager::CheckLevelCompletionConditions(const FName& LevelName) const
{
	FLevelInfo LevelInfo = GetLevelInfo(LevelName);
	
	// 현재는 기본적으로 완료된 것으로 간주
	// 향후 실제 완료 조건 로직 구현 예정
	return LevelInfo.ProgressState == ELevelProgressState::Completed;
}

// === 층별 진행 관리 함수들 ===

int32 UMatrixLevelManager::GetCurrentFloor() const
{
    return CurrentFloor;
}

void UMatrixLevelManager::SetFloorWaveCompleted(int32 FloorNumber, bool bCompleted)
{
    FloorWaveCompletionStatus.Add(FloorNumber, bCompleted);
    UE_LOG(LogTemp, Log, TEXT("Floor %d wave completion status set to: %s"), 
           FloorNumber, bCompleted ? TEXT("Completed") : TEXT("Not Completed"));
    
    // 1층 웨이브가 완료되면 게임 클리어 조건 체크
    if (FloorNumber == 1 && bCompleted)
    {
        if (CheckGameClearConditions())
        {
            UE_LOG(LogTemp, Log, TEXT("Game Clear conditions met!"));
            RequestLevelTransition(GameClearLevel, ELevelTransitionType::GameClear);
        }
    }
}

bool UMatrixLevelManager::IsFloorWaveCompleted(int32 FloorNumber) const
{
    const bool* bCompleted = FloorWaveCompletionStatus.Find(FloorNumber);
    return bCompleted ? *bCompleted : false;
}

bool UMatrixLevelManager::CanProceedToNextFloor() const
{
    // 현재 층의 웨이브가 완료되었는지 확인
    return IsFloorWaveCompleted(CurrentFloor);
}

bool UMatrixLevelManager::CanProceedToNextFloorWithWaveCheck() const
{
    // 게임 모드에서 실제 웨이브 진행 상태 확인
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    
    AMatrixGameMode* GameMode = Cast<AMatrixGameMode>(World->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode not found for wave check"));
        return false;
    }
    
    // 현재 층에 해당하는 웨이브 데이터 테이블 확인
    UDataTable* CurrentWaveDataTable = nullptr;
    
    // 층별 웨이브 데이터 테이블 매핑
    if (CurrentFloor == 2)
    {
        CurrentWaveDataTable = GameMode->GetWaveDataTable();
    }
    else if (CurrentFloor == 1)
    {
        // 1층용 웨이브 데이터 테이블이 있다면 여기서 설정
        CurrentWaveDataTable = GameMode->GetWaveDataTable();
    }
    
    if (!CurrentWaveDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wave data table not found for floor %d"), CurrentFloor);
        return false;
    }
    
    // 현재 웨이브 번호와 총 웨이브 수 비교
    AMatrixGameState* GameState = Cast<AMatrixGameState>(World->GetGameState());
    if (!GameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameState not found for wave check"));
        return false;
    }
    
    int32 CurrentWave = GameState->CurrentWave;
    int32 TotalWaves = CurrentWaveDataTable->GetRowNames().Num();
    
    UE_LOG(LogTemp, Log, TEXT("Floor %d: Current Wave %d / Total Waves %d"), 
           CurrentFloor, CurrentWave, TotalWaves);
    
    // 현재 웨이브가 총 웨이브 수보다 크면 모든 웨이브 완료
    bool bAllWavesCompleted = (CurrentWave > TotalWaves);
    
    // 추가로 남은 적이 있는지 확인
    bool bNoEnemiesRemaining = (GameState->EnemiesRemaining <= 0);
    
    bool bCanProceed = bAllWavesCompleted && bNoEnemiesRemaining;
    
    UE_LOG(LogTemp, Log, TEXT("Floor %d progression check: AllWavesCompleted=%s, NoEnemiesRemaining=%s, CanProceed=%s"), 
           CurrentFloor, 
           bAllWavesCompleted ? TEXT("True") : TEXT("False"),
           bNoEnemiesRemaining ? TEXT("True") : TEXT("False"),
           bCanProceed ? TEXT("True") : TEXT("False"));
    
    return bCanProceed;
}

void UMatrixLevelManager::ProceedToNextFloor()
{
    if (!CanProceedToNextFloorWithWaveCheck())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot proceed to next floor. Current floor %d waves not completed."), CurrentFloor);
        return;
    }
    
    // 현재 층의 Blocking Volume 제거 (2층을 언로드하지 않음)
    DisableFloorBlockingVolumes(CurrentFloor);
    UE_LOG(LogTemp, Log, TEXT("Disabled blocking volumes for floor %d"), CurrentFloor);
    
    // 다음 층으로 이동
    CurrentFloor--;
    
    // 1층으로 이동 시 1층 서브레벨이 이미 로드되어 있으므로 추가 로드 불필요
    UE_LOG(LogTemp, Log, TEXT("Proceeded to floor %d (sublevel already loaded)"), CurrentFloor);
}

bool UMatrixLevelManager::CheckGameClearConditions() const
{
    // 1층 웨이브가 완료되었는지 확인
    if (!IsFloorWaveCompleted(1))
    {
        return false;
    }
    
    // 1층에서 스폰된 모든 적이 사망했는지 확인
    // 이 부분은 GameMode나 GameState에서 적 상태를 확인해야 함
    // 현재는 1층 웨이브 완료만으로 게임 클리어 조건 충족으로 간주
    
    UE_LOG(LogTemp, Log, TEXT("Game clear conditions checked - Floor 1 waves completed"));
    return true;
}

// === Blocking Volume 관리 함수들 ===

void UMatrixLevelManager::RegisterFloorBlockingVolume(int32 FloorNumber, AActor* BlockingVolume)
{
    if (!BlockingVolume)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to register null blocking volume for floor %d"), FloorNumber);
        return;
    }
    
    if (!FloorBlockingVolumes.Contains(FloorNumber))
    {
        FloorBlockingVolumes.Add(FloorNumber, TArray<AActor*>());
    }
    
    FloorBlockingVolumes[FloorNumber].Add(BlockingVolume);
    UE_LOG(LogTemp, Log, TEXT("Registered blocking volume %s for floor %d"), *BlockingVolume->GetName(), FloorNumber);
}

void UMatrixLevelManager::RemoveFloorBlockingVolumes(int32 FloorNumber)
{
    if (!FloorBlockingVolumes.Contains(FloorNumber))
    {
        UE_LOG(LogTemp, Warning, TEXT("No blocking volumes found for floor %d"), FloorNumber);
        return;
    }
    
    TArray<AActor*>& BlockingVolumes = FloorBlockingVolumes[FloorNumber];
    
    for (AActor* Volume : BlockingVolumes)
    {
        if (Volume && Volume->IsValidLowLevel())
        {
            Volume->Destroy();
            UE_LOG(LogTemp, Log, TEXT("Destroyed blocking volume %s for floor %d"), *Volume->GetName(), FloorNumber);
        }
    }
    
    BlockingVolumes.Empty();
    UE_LOG(LogTemp, Log, TEXT("Removed all blocking volumes for floor %d"), FloorNumber);
}

void UMatrixLevelManager::DisableFloorBlockingVolumes(int32 FloorNumber)
{
    if (!FloorBlockingVolumes.Contains(FloorNumber))
    {
        UE_LOG(LogTemp, Warning, TEXT("No blocking volumes found for floor %d"), FloorNumber);
        return;
    }
    
    TArray<AActor*>& BlockingVolumes = FloorBlockingVolumes[FloorNumber];
    
    for (AActor* Volume : BlockingVolumes)
    {
        if (Volume && Volume->IsValidLowLevel())
        {
            Volume->SetActorHiddenInGame(true);
            Volume->SetActorEnableCollision(false);
            UE_LOG(LogTemp, Log, TEXT("Disabled blocking volume %s for floor %d"), *Volume->GetName(), FloorNumber);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Disabled all blocking volumes for floor %d"), FloorNumber);
}
