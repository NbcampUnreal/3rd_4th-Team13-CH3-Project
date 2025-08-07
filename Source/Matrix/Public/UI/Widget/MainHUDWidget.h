#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class MATRIX_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmoText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxAmmoText;

	UPROPERTY(meta =(BindWidgetAnim), Transient)
	UWidgetAnimation* PunchAmmoScale;
};
