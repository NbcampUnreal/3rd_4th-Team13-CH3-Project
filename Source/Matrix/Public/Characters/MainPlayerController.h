#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UMainHUDWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UMainHUDWidget* HUDWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UWeaponHUDWidget> WeaponHUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UWeaponHUDWidget* WeaponHUDWidgetInstance;

	virtual void SetupInputComponent() override;

	// �Ͻ�����  UI On/Off �Լ�
	void HandlePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void NotifyAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameClearWidgetClass;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGameStateChanged(EGameState NewState);

private:
	// ���� ������ Pause �������� Ȯ���ϴ� ����
	bool bIsPaused = false;

	UPROPERTY(EditAnywhere, Category = "Menu")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;

	UPROPERTY()
	UUserWidget* CurrentScreenWidget; // Pause, GameOver, GameClear 등 기존 화면에 덮는 위젯 변수입니다!
};
