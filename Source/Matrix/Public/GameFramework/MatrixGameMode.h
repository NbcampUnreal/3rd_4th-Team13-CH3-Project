#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MatrixGameMode.generated.h"

class AMatrixGameState;
class AEnemyCharacter;
class ATargetPoint;

UCLASS()
class MATRIX_API AMatrixGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMatrixGameMode();
    
protected:
	virtual void BeginPlay() override;
	void StartWave();
	void EndWave();

public:
	void EnemyKilled();

protected:
	// 웨이브 사이의 대기 시간 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	float TimeBetweenWaves = 10.0f;
    
	// 스폰할 적 클래스 
	UPROPERTY(EditDefaultsOnly, Category="Game Rule")
	TSubclassOf<AEnemyCharacter> EnemyToSpawnClass;

private:
	FTimerHandle TimerHandle_NextWaveStart;
    
	UPROPERTY()
	AMatrixGameState* MatrixGameState;
};