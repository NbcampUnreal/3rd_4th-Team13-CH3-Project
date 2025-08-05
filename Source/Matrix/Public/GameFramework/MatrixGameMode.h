#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MatrixGameMode.generated.h"

class AMatrixGameState;
class AEnemyCharacter;
class ATargetPoint;

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere)
	TArray<ATargetPoint*> SpawnPoints;
};

USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 EnemiesToSpawn = 0;

	UPROPERTY(EditAnywhere)
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

protected:
	// 웨이브 사이의 대기 시간 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	float TimeBetweenWaves = 10.0f;
    
	// 스폰할 적 클래스 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	TSubclassOf<AEnemyCharacter> EnemyToSpawnClass;
	
	virtual void BeginPlay() override;
	void StartWave();
	void EndWave();
	void StartNextLevel();

private:
	FTimerHandle TimerHandle_NextWaveStart;
    
	UPROPERTY()
	AMatrixGameState* MatrixGameState;

	UPROPERTY(EditAnywhere, Category = "Game System")
	UDataTable* LevelDataTable;

	UDataTable* CurrentWaveDataTable;
};