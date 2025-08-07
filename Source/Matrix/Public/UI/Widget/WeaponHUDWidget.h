#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapons/WeaponSystem/WeaponBase.h"
#include "WeaponHUDWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class MATRIX_API UWeaponHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWeaponIcon(EWeaponType WeaponType);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponHUD")
	UTexture2D* NoneIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponHUD")
	UTexture2D* PistolIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponHUD")
	UTexture2D* RifleIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponHUD")
	UTexture2D* ShotgunIcon;
};
