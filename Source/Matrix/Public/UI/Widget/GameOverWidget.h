#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Textblock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "GameOverWidget.generated.h"

UCLASS()
class MATRIX_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Logo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Wave;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Result;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Reason;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TotalCount;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Title;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Quit;

	UPROPERTY(meta = (BindWidget))
	UBorder* Br_Title;
	UPROPERTY(meta = (BindWidget))
	UBorder* Br_Quit;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* LogoAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PunchCount;

	UFUNCTION()
	void OnTitleClicked();
	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnTitleFocused();
	UFUNCTION()
	void OnQuitFocused();
	UFUNCTION()
	void OnTitleUnfocused();
	UFUNCTION()
	void OnQuitUnfocused();
};
