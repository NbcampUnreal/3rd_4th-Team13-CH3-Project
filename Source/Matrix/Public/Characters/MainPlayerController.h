#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widget/MainMenuWidget.h"
#include "UI/Widget/GameOverWidget.h"
#include "UI/Widget/GameClearWidget.h"
#include "UI/Widget/MainHUDWidget.h"
#include "UI/Widget/WeaponHUDWidget.h"
#include "MainPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class MATRIX_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainPlayerController();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UMainMenuWidget* MainMenuWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UMainHUDWidget* MainHUDWidgetInstance;

	virtual void SetupInputComponent() override;

	// 일시정지 UI On/Off 함수
	void HandlePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void NotifyAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameClearWidget> GameClearWidgetClass;
	
	// === 게임 흐름 관리 함수들 (간소화) ===
	
	// 게임 시작/끝 관리
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
	
	// 레벨 전환 관리
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RequestLevelTransition(const FName& TargetLevel);
	
	// 현재 게임 상태 확인
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool IsInGame() const;
	
	// === 테스트용 함수들 ===
	
	// 게임 오버 테스트
	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestGameOver();
	
	// 게임 클리어 테스트
	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestGameClear();
	
	// 다음 레벨 테스트
	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestNextLevel();

	UFUNCTION(BlueprintCallable, Category = "Options")
	void OpenOptions();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGameStateChanged(EGameState NewState);

private:
	// 일시정지 상태를 Pause 확인하는 변수
	bool bIsPaused = false;

	UPROPERTY(EditAnywhere, Category = "Menu")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;

	UPROPERTY()
	UUserWidget* CurrentScreenWidget; // Pause, GameOver, GameClear 등 기존 화면에 덮는 위젯 변수입니다!


};
