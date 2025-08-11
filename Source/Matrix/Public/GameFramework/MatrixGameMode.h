#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "MatrixGameMode.generated.h"

class AMatrixGameState;
class AEnemyCharacter;

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SpawnPointTag;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount = 1;
};

USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpawnInfo> SpawnInfos;
};

USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UDataTable* WaveDataTable;
};

UCLASS()
class MATRIX_API AMatrixGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMatrixGameMode();
	void EnemyKilled();
	void PlayerDied();
	void RequestTogglePause();

protected:
	// 웨이브 사이의 대기 시간 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	float TimeBetweenWaves = 10.0f;
    
	virtual void BeginPlay() override;
	void StartWave();
	void EndWave();
	void PrepareNextLevel();

private:
	FTimerHandle TimerHandle_NextWaveStart;
    
	UPROPERTY()
	AMatrixGameState* MatrixGameState;

	UPROPERTY(EditAnywhere, Category = "Game System")
	UDataTable* LevelDataTable;

	UDataTable* CurrentWaveDataTable;

	UPROPERTY()
	class AMatrixSpawnManager* SpawnManager;

	void CheckGameClearCondition();
};