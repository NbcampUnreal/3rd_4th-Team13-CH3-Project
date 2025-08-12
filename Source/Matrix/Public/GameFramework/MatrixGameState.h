#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Core/MatrixCoreTypes.h"
#include "MatrixGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewState);

UCLASS()
class MATRIX_API AMatrixGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMatrixGameState();

	UPROPERTY(BlueprintReadOnly, Replicated, Category="Game State")
	int32 CurrentWave = 0;

	UPROPERTY(BlueprintReadOnly, Replicated, Category="Game State")
	int32 EnemiesRemaining = 0;

	void SetGameState(EGameState NewState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_GameState, Category="Game State")
	EGameState CurrentGameState;

	UPROPERTY(BlueprintAssignable, Category = "Game State")
	FOnGameStateChanged OnGameStateChanged;

protected:
	UFUNCTION()
	void OnRep_GameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};