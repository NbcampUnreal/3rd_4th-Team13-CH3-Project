#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenuWidget.generated.h"

class UButton;
class USlider;
class USoundClass;
class UMainPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackPressed);

UCLASS()
class MATRIX_API UOptionsMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = "Options|Event")
	FOnBackPressed OnBackPressed;

	UFUNCTION(BlueprintCallable, Category = "Options")
	void ApplySettings();
	UFUNCTION(BlueprintCallable, Category = "Options")
	void OnCancelClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	USlider* Slider_Sensitivity;
	UPROPERTY(meta = (BindWidget))
	USlider* Slider_Brightness;
	UPROPERTY(meta = (BindWidget))
	USlider* Slider_Volume;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Apply;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Cancel;
	
	UFUNCTION()
	void OnSliderSensitivityChanged(float Value);
	UFUNCTION()
	void OnSliderBrightnessChanged(float Value);
	UFUNCTION()
	void OnSliderVolumeChanged(float Value);

private:
	UPROPERTY(EditAnywhere, Category = "Options")
	float CurrentSensitivity = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Options")
	float CurrentBrightness = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Options")
	float CurrentVolume = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Options|Audio")
	USoundClass* MasterSoundClass = nullptr;
};
