#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MatrixGameState.generated.h"

UCLASS()
class MATRIX_API AMatrixGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Game State")
	int32 CurrentWave = 0;

	UPROPERTY(BlueprintReadOnly, Category="Game State")
	int32 EnemiesRemaining = 0;
};