#pragma once

#include "CoreMinimal.h"
#include "MatrixCoreTypes.generated.h"

// === Game State Management ===

UENUM(BlueprintType)
enum class EGameState : uint8
{
    MainMenu,       // 메인 메뉴
    Playing,        // 게임 플레이 중
    Paused,         // 일시정지
    GameOver,       // 게임 오버
    GameClear       // 게임 클리어
};

// === Game Flow Related Types ===

UENUM(BlueprintType)
enum class ELevelType : uint8
{
    MainMenu,       // 메인 메뉴 레벨
    Gameplay,       // 게임플레이 레벨
    Boss,           // 보스 레벨
    Environment,    // 환경 레벨
    SubLevel        // 서브 레벨
};

UENUM(BlueprintType)
enum class ELevelProgressState : uint8
{
    NotStarted,     // 시작하지 않음
    InProgress,     // 진행 중
    Completed,      // 완료됨
    Failed          // 실패
};

// === Trigger System Related Types ===

UENUM(BlueprintType)
enum class EMatrixTriggerType : uint8
{
    LevelTransition,    // 레벨 전환
    SubLevelLoad,       // 서브레벨 로드
    SubLevelUnload,     // 서브레벨 언로드
    WaveStart,          // 웨이브 시작
    DoorOpen,           // 문 열기
    Custom              // 커스텀 액션
};

UENUM(BlueprintType)
enum class ETriggerCondition : uint8
{
    OnEnter,        // 진입 시
    OnExit,         // 나갈 시
    OnStay,         // 머물 때
    OnInteract      // 상호작용 시
};

// === Struct Definitions ===

USTRUCT(BlueprintType)
struct FLevelInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Info")
    FName LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Info")
    ELevelType LevelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Info")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Info")
    TArray<FName> SubLevels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Info")
    bool bIsUnlocked = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Info")
    ELevelProgressState ProgressState = ELevelProgressState::NotStarted;

    FLevelInfo()
    {
        LevelName = NAME_None;
        LevelType = ELevelType::Gameplay;
        DisplayName = TEXT("");
        SubLevels.Empty();
        bIsUnlocked = true;
        ProgressState = ELevelProgressState::NotStarted;
    }
};

USTRUCT(BlueprintType)
struct FTriggerInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    EMatrixTriggerType TriggerType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    ETriggerCondition TriggerCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    FName TargetLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    FName SubLevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    int32 WaveNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    float Delay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    bool bOneTimeUse = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Info")
    FString CustomEventName;

    FTriggerInfo()
    {
        TriggerType = EMatrixTriggerType::Custom;
        TriggerCondition = ETriggerCondition::OnEnter;
        TargetLevel = NAME_None;
        SubLevelName = NAME_None;
        WaveNumber = 1;
        Delay = 0.0f;
        bOneTimeUse = true;
        CustomEventName = TEXT("");
    }
};
