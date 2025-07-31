#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MatrixGameMode.generated.h"

UCLASS()
class MATRIX_API AMatrixGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	void EnemyKilled();

private:
	void StartWave();

	void EndWave();

	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	float TimeBetweenWaves = 5.0f;

	FTimerHandle TimerHandle_NextWaveStart;

	int32 NumEnemiesToSpawn;
};
