#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MatrixGameInstance.generated.h"

UCLASS()
class MATRIX_API UMatrixGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Data")
	int32 PlayerScore = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Game Data")
	int32 CurrentLevel = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Game Data")
	int32 MaxLevel = 2;

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void AddScore(int32 ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void ResetGameData();

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void AdvanceToNextLevel();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void GoToMainMenu();

	void AdvanceToNextStreamingLevel();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName MainGameLevelName;

	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName MainMenuLevelName;
};
