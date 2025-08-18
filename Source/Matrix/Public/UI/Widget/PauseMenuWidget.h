#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/MatrixCoreTypes.h"
#include "PauseMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuContinue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPauseMenuOptions, EGameState, FromState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuMainMenu);

class UButton;
class UWidgetAnimation;

UCLASS()
class MATRIX_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Continue = nullptr;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Options = nullptr;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_MainMenu = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Transform_GuidanceText = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "PauseMenu|Event")
	FOnPauseMenuContinue OnPauseMenuContinue;
	UPROPERTY(BlueprintAssignable, Category = "PauseMenu|Event")
	FOnPauseMenuOptions OnPauseMenuOptions;
	UPROPERTY(BlueprintAssignable, Category = "PauseMenu|Event")
	FOnPauseMenuMainMenu OnPauseMenuMainMenu;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void RequestResume();

private:
	UFUNCTION()
	void HandleContinueClicked();
	UFUNCTION()
	void HandleOptionsClicked();
	UFUNCTION()
	void HandleMainMenuClicked();
};
