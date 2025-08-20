#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widget/MainMenuWidget.h"
#include "UI/Widget/OptionsMenuWidget.h"
#include "UI/Widget/PauseMenuWidget.h"
#include "UI/Widget/GameOverWidget.h"
#include "UI/Widget/GameClearWidget.h"
#include "UI/Widget/MainHUDWidget.h"
#include "UI/Widget/WeaponHUDWidget.h"
#include "UI/Widget/SlotHUDWidget.h"
#include "MainPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class MATRIX_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainPlayerController();
	
	// === Input Properties ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* QuickSlot1Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* QuickSlot2Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* QuickSlot3Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SlowTimeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options|Input")
	float MouseSensitivity = 1.0f;

	// === UI Widget Classes ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UOptionsMenuWidget> OptionsMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameClearWidget> GameClearWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

	// === UI Widget Instances ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UMainMenuWidget* MainMenuWidgetInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UOptionsMenuWidget* OptionsMenuWidgetInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UPauseMenuWidget* PauseMenuWidgetInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UMainHUDWidget* MainHUDWidgetInstance;

	// === Core Functions ===
	virtual void SetupInputComponent() override;
	virtual void AddYawInput(float Val) override;
	virtual void AddPitchInput(float Val) override;

	// === Game State Management ===
	UFUNCTION()
	void OnGameStateChanged(EGameState NewState);

	// === Input Handling ===
	void HandlePauseMenu();

	// === HUD Updates ===
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void NotifyAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void NotifyKillCountChanged(int32 KillCount);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void HandleInventoryUpdated();

	// === Game Flow Management ===
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EndGame(const FString& EndReason = "Normal");
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void GoToMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RestartGame();
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void OnMenuExit();
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RequestLevelTransition(const FName& TargetLevel);
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool IsInGame() const;

	// === Options Menu ===
	UFUNCTION(BlueprintCallable, Category = "Options")
	void OpenOptions(EGameState FromState);
	UFUNCTION(BlueprintCallable, Category = "Options")
	void CloseOptions();

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	// === Test Functions (Development Only) ===
	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestGameOver();
	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestGameClear();
	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestNextLevel();
#endif

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UUserWidget* CurrentScreenWidget;
	UPROPERTY()
	EGameState PreviousState;
};
