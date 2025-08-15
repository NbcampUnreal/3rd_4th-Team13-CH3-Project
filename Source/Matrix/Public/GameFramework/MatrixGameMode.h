#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Core/MatrixCoreTypes.h"
#include "Core/MatrixWaveTypes.h"
#include "MatrixGameMode.generated.h"

class AMatrixGameState;
class AMatrixSpawnManager;

UCLASS()
class MATRIX_API AMatrixGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMatrixGameMode();
	void EnemyKilled();
	void PlayerDied();
	void RequestTogglePause();
	void RegisterSpawnManager(AMatrixSpawnManager* InSpawnManager);
	
	// 웨이브 관리 함수들
	UFUNCTION(BlueprintCallable, Category = "Wave Management")
	void StartSpecificWave(int32 WaveNumber);
	
	UFUNCTION(BlueprintCallable, Category = "Wave Management")
	void ForceStartWave();

protected:
	// 웨이브 사이의 대기 시간 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	float TimeBetweenWaves = 10.0f;
    
	virtual void BeginPlay() override;
	void StartWave();
	void EndWave();
	void CheckBossStageOrGameClear();
	void StartBossStage();

private:
	FTimerHandle TimerHandle_NextWaveStart;
    
	UPROPERTY()
	AMatrixGameState* MatrixGameState;

	UPROPERTY(EditAnywhere, Category = "Game System")
	UDataTable* WaveDataTable;

	UDataTable* CurrentWaveDataTable;

	UPROPERTY()
	class AMatrixSpawnManager* SpawnManager;

	// 보스 스테이지 관련 설정
	UPROPERTY(EditAnywhere, Category = "Boss Stage")
	bool bHasBossStage = false;

	UPROPERTY(EditAnywhere, Category = "Boss Stage")
	FName BossStageLevelName = TEXT("S_Boss");
};