#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MediaPlayer.h"
#include "FileMediaSource.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/MatrixCoreTypes.h"
#include "MainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuStartRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuOptionsRequested, EGameState, FromState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuExitRequested);

UCLASS()
class MATRIX_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UButton* Button_StartGame = nullptr;
    UPROPERTY(meta = (BindWidget))
    UButton* Button_Options = nullptr;
    UPROPERTY(meta = (BindWidget))
    UButton* Button_Exit = nullptr;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainMenu")
    UMediaPlayer* MenuMediaPlayer = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainMenu")
    UFileMediaSource* MenuMediaSource = nullptr;

    UPROPERTY(BlueprintAssignable, Category = "Menu|Event")
    FOnMenuStartRequested OnStartRequested;
    UPROPERTY(BlueprintAssignable, Category = "Menu|Event")
    FOnMenuOptionsRequested OnOptionsRequested;
    UPROPERTY(BlueprintAssignable, Category = "Menu|Event")
    FOnMenuExitRequested OnExitRequested;

protected:
    virtual void NativeConstruct() override; // AddToViewport 시 호출
    virtual void NativeDestruct() override;  // RemoveFromParent 시 호출

private:
    UFUNCTION()
    void HandleStartClicked();
    UFUNCTION()
    void HandleOptionsClicked();
    UFUNCTION()
    void HandleExitClicked();
};
