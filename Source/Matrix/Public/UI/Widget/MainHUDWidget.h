#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MainHUDWidget.generated.h"

UCLASS()
class MATRIX_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta= (BindWidget))
	UTextBlock* AmmoText;

	UFUNCTION()
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
};
