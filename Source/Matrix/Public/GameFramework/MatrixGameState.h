// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MatrixGameState.generated.h"

class AEnemyCharacter;

/**
 * 
 */
UCLASS()
class MATRIX_API AMatrixGameState : public AGameState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Spawning")
	TSubclassOf<AEnemyCharacter> EnemyToSpawnClass;

	virtual void BeginPlay() override;

public:
	AMatrixGameState();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	int32 CurrentWave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	int32 EnemiesRemaining;

	void StartWave();
	void EnemyKilled();
	void EndWave();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	float TimeBetweenWaves = 5.0f;

	FTimerHandle TimerHandle_NextWaveStart;
};
