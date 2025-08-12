#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/MatrixCoreTypes.h"
#include "MatrixLevelManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelTransitionStarted, const FName&, LevelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelTransitionCompleted, const FName&, LevelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStarted, const FName&, StartLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, const FString&, EndReason);

UENUM(BlueprintType)
enum class ELevelTransitionType : uint8
{
    MainMenu,       // 메인 메뉴로
    GameStart,      // 게임 시작
    GameOver,       // 게임 오버
    GameClear,      // 게임 클리어
    NextLevel,      // 다음 레벨
    Restart         // 재시작
};

UCLASS()
class MATRIX_API UMatrixLevelManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMatrixLevelManager();

    // === 게임 시작/끝 관리 ===
    
    // 게임 시작
    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void StartGame();
    
    // 게임 종료
    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void EndGame(const FString& EndReason = "Normal");
    
    // 메인 메뉴로 돌아가기
    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void GoToMainMenu();
    
    // 게임 재시작
    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void RestartGame();
    
    // === 레벨 전환 관리 ===
    
    // 레벨 전환 요청
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void RequestLevelTransition(const FName& TargetLevel, ELevelTransitionType TransitionType);
    
    // 서브레벨 로드/언로드
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void LoadSubLevel(const FName& SubLevelName, bool bMakeVisibleAfterLoad = true);
    
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void UnloadSubLevel(const FName& SubLevelName);
    
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    bool IsSubLevelLoaded(const FName& SubLevelName) const;
    
    // 현재 레벨 정보
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    FName GetCurrentLevel() const;
    
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    bool IsInGame() const;
    
    // === 레벨 정보 관리 ===
    
    // 레벨 정보 가져오기
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    FLevelInfo GetLevelInfo(const FName& LevelName) const;
    
    // 현재 레벨 타입 가져오기
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    ELevelType GetCurrentLevelType() const;
    
    // 레벨 완료 조건 체크
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    bool CheckLevelCompletionConditions(const FName& LevelName) const;
    
    // === 레벨 설정 ===
    
    // 레벨 정보 테이블 (에디터에서 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    TArray<FLevelInfo> LevelInfos;
    
    // === 이벤트 델리게이트 ===
    
    // 레벨 전환 시작
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnLevelTransitionStarted OnLevelTransitionStarted;
    
    // 레벨 전환 완료
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnLevelTransitionCompleted OnLevelTransitionCompleted;
    
    // 게임 시작
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameStarted OnGameStarted;
    
    // 게임 종료
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameEnded OnGameEnded;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    // === 레벨 설정 ===
    
    // 메인 메뉴 레벨
    UPROPERTY(EditDefaultsOnly, Category = "Level Settings")
    FName MainMenuLevel = TEXT("MainMenu");
    
    // 게임 시작 레벨
    UPROPERTY(EditDefaultsOnly, Category = "Level Settings")
    FName GameStartLevel = TEXT("P_MainMap");
    
    // 게임 오버 레벨
    UPROPERTY(EditDefaultsOnly, Category = "Level Settings")
    FName GameOverLevel = TEXT("GameOver");
    
    // 게임 클리어 레벨
    UPROPERTY(EditDefaultsOnly, Category = "Level Settings")
    FName GameClearLevel = TEXT("GameClear");
    
    // === 상태 관리 ===
    
    // 현재 레벨
    UPROPERTY()
    FName CurrentLevel;
    
    // 게임 진행 중인지
    UPROPERTY()
    bool bIsInGame = false;
    
    // 레벨 전환 중인지
    UPROPERTY()
    bool bIsTransitioning = false;
    
    // 게임 시작 시간
    UPROPERTY()
    float GameStartTime = 0.0f;
    
    // === 내부 함수들 ===
    
    // 실제 레벨 전환 실행
    void ExecuteLevelTransition(const FName& TargetLevel, ELevelTransitionType TransitionType);
    
    // 레벨 전환 완료 처리
    void OnLevelTransitionFinished();
    
    // 서브레벨 로드/언로드 완료 콜백
    void OnSubLevelLoaded(const FName& SubLevelName);
    void OnSubLevelUnloaded(const FName& SubLevelName);
    
    // 게임 상태 초기화
    void InitializeGameState();
    
    // 게임 상태 정리
    void CleanupGameState();
};
