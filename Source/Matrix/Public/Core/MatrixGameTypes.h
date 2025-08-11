#pragma once

#include "CoreMinimal.h"
#include "MatrixGameTypes.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	None,
	MainMenu,
	Loading,
	Playing,
	Paused,
	GameOver,
	GameClear
};

// === 게임 흐름 관련 타입들 ===

// 레벨 타입
UENUM(BlueprintType)
enum class ELevelType : uint8
{
	MainMenu,       // 메인 메뉴
	Gameplay,       // 일반 게임플레이
	Boss,           // 보스 전투
	Environment,    // 환경/배경
	Transition      // 전환 레벨
};

// 게임플레이 이벤트 타입
UENUM(BlueprintType)
enum class EGameplayEventType : uint8
{
	EnemySpawned,       // 적 스폰
	EnemyDefeated,      // 적 처치
	PlayerDamaged,      // 플레이어 피해
	PlayerHealed,       // 플레이어 회복
	WeaponChanged,      // 무기 변경
	LevelStarted,       // 레벨 시작
	LevelCompleted,     // 레벨 완료
	BossFightStarted,   // 보스 전투 시작
	BossFightEnded,     // 보스 전투 종료
	WaveStarted,        // 웨이브 시작
	WaveCompleted,      // 웨이브 완료
	GameStarted,        // 게임 시작
	GameEnded           // 게임 종료
};

// 레벨 진행 상태
UENUM(BlueprintType)
enum class ELevelProgressState : uint8
{
	NotStarted,     // 시작하지 않음
	InProgress,     // 진행 중
	Completed,      // 완료
	Failed          // 실패
};



// === 구조체 정의 ===

// 레벨 정보
USTRUCT(BlueprintType)
struct FLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELevelType LevelType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLimit = 0.0f; // 0 = 무제한

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredSubLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequireBossDefeat = false;
};

// 게임플레이 이벤트 데이터
USTRUCT(BlueprintType)
struct FMatrixGameplayEventData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameplayEventType EventType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EventData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SourceActor;
};