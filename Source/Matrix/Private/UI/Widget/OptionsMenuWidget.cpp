#include "UI/Widget/OptionsMenuWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MainPlayerController.h"
#include "GameFramework/MatrixGameInstance.h"

static float MapBrightnessToAutoExposure(const float Brightness)
{
	return (Brightness * 4.0f) - 2.0f; // → -2.0f ~ 2.0f
}

void UOptionsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UMatrixGameInstance* GameInstance = GetGameInstance<UMatrixGameInstance>())
	{
		CurrentSensitivity = GameInstance->SavedSensitivity;
		CurrentBrightness = GameInstance->SavedBrightness;
	}

	if (Slider_Sensitivity) 
	{ 
		Slider_Sensitivity->OnValueChanged.AddDynamic(this, &UOptionsMenuWidget::OnSliderSensitivityChanged);
		Slider_Sensitivity->SetValue(CurrentSensitivity);
	}

	if (Slider_Brightness) 
	{ 
		Slider_Brightness->OnValueChanged.AddDynamic(this, &UOptionsMenuWidget::OnSliderBrightnessChanged); 
		Slider_Brightness->SetValue(CurrentBrightness);
	}

/*	if (Slider_Volume)
	{ 
		Slider_Volume->OnValueChanged.AddDynamic(this, &UOptionsMenuWidget::OnSliderVolumeChanged); 
		Slider_Volume->SetValue(CurrentVolume);
	}
*/	
	if (Button_Apply) { Button_Apply->OnClicked.AddDynamic(this, &UOptionsMenuWidget::ApplySettings); }
	if (Button_Cancel) { Button_Cancel->OnClicked.AddDynamic(this, &UOptionsMenuWidget::OnCancelClicked); }

	ApplySettings();
}

void UOptionsMenuWidget::OnSliderSensitivityChanged(float Value) { CurrentSensitivity = Value; }
void UOptionsMenuWidget::OnSliderBrightnessChanged(float Value) { CurrentBrightness = Value; }
//void UOptionsMenuWidget::OnSliderVolumeChanged(float Value) {	const float Perceived = FMath::Pow(CurrentVolume, 2.2f);


void UOptionsMenuWidget::ApplySettings()
{
	if (UMatrixGameInstance* GameInstance = GetGameInstance<UMatrixGameInstance>())
	{
		GameInstance->SavedSensitivity = CurrentSensitivity;
		GameInstance->SavedBrightness = CurrentBrightness;
		//GameInstance->SavedVolume = CurrentVolume; // 볼륨은 현재 주석 처리
	}


	 //마스터 볼륨 적용
/*	if (MasterSoundClass)
	{
		MasterSoundClass->Properties.Volume = CurrentVolume;
	}
*/

	//밝기 적용 (PostProcessVolume -> AutoExposureBias)
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> FoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(World, APostProcessVolume::StaticClass(), FoundVolumes);

		const float Bias = MapBrightnessToAutoExposure(CurrentBrightness);

		for (AActor* Actor : FoundVolumes)
		{
			APostProcessVolume* PPV = Cast<APostProcessVolume>(Actor);
			if (PPV)
			{
				PPV->Settings.AutoExposureBias = Bias;
			}
		}
	}

	// 민감도 적용 (PlayerController 전달)
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
		{
			MPC->MouseSensitivity = CurrentSensitivity;
		}
	}
}

void UOptionsMenuWidget::OnCancelClicked()
{
	if (const UMatrixGameInstance* GameInstance = GetGameInstance<UMatrixGameInstance>())
	{
		CurrentSensitivity = GameInstance->SavedSensitivity;
		CurrentBrightness = GameInstance->SavedBrightness;
	}

	if (Slider_Sensitivity)
	{
		Slider_Sensitivity->SetValue(CurrentSensitivity);
	}

	if (Slider_Brightness)
	{
		Slider_Brightness->SetValue(CurrentBrightness);
	}

	if(OnBackPressed.IsBound())
	{
		OnBackPressed.Broadcast();
	}
}
