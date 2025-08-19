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
	
	// 웨이브 데이터 테이블 반환
	UFUNCTION(BlueprintCallable, Category = "Wave Management")
	UDataTable* GetWaveDataTable() const { return CurrentWaveDataTable; }

protected:
	// 웨이브 사이의 대기 시간 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	float TimeBetweenWaves = 10.0f;
    
	virtual void BeginPlay() override;
	void StartWave();
	void EndWave();
	void HandleFloorWaveCompletion();
	void InitializeLevelStreaming();
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

	// 머티리얼 오버라이드 설정
	UPROPERTY(EditAnywhere, Category = "Material Override")
	bool bApplyWhiteMaterialOnStart = true; // 게임 시작 시 하얀색 머티리얼 적용 여부
};