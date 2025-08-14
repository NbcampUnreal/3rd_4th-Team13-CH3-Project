#include "GameFramework/MatrixLevelManager.h"
#include "GameFramework/MatrixGameState.h"
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
}

void UMatrixLevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("MatrixLevelManager initialized"));
    
    // 현재 레벨 설정
    if (UWorld* World = GetWorld())
    {
        CurrentLevel = FName(*World->GetMapName());
        UE_LOG(LogTemp, Log, TEXT("Current level: %s"), *CurrentLevel.ToString());
    }
}

void UMatrixLevelManager::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("MatrixLevelManager deinitialized"));
    Super::Deinitialize();
}

// === 게임 시작/끝 관리 ===

void UMatrixLevelManager::StartGame()
{
    if (bIsInGame)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game is already started"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Starting game..."));
    
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
        UE_LOG(LogTemp, Warning, TEXT("Game is not started"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Ending game. Reason: %s"), *EndReason);
    
    // 게임 종료 이벤트 브로드캐스트
    OnGameEnded.Broadcast(EndReason);
    
    // 게임 상태 정리
    CleanupGameState();
    
    // 메인 메뉴로 돌아가기
    GoToMainMenu();
}

void UMatrixLevelManager::GoToMainMenu()
{
    UE_LOG(LogTemp, Log, TEXT("Going to main menu"));
    RequestLevelTransition(MainMenuLevel, ELevelTransitionType::MainMenu);
}

void UMatrixLevelManager::RestartGame()
{
    UE_LOG(LogTemp, Log, TEXT("Restarting game"));
    
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
    
    UE_LOG(LogTemp, Log, TEXT("Requesting level transition to: %s (Type: %d)"), 
           *TargetLevel.ToString(), (int32)TransitionType);
    
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
    UE_LOG(LogTemp, Log, TEXT("Level transition finished"));
}

// === 서브레벨 스트리밍 함수들 ===

void UMatrixLevelManager::LoadSubLevel(const FName& SubLevelName, bool bMakeVisibleAfterLoad)
{
    if (SubLevelName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("SubLevel name is None"));
        return;
    }
    
    if (IsSubLevelLoaded(SubLevelName))
    {
        UE_LOG(LogTemp, Warning, TEXT("SubLevel %s is already loaded"), *SubLevelName.ToString());
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Loading sublevel: %s"), *SubLevelName.ToString());
    
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
