#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/WeaponHUDWidget.h"
#include "MainHUDWidget.generated.h"


class ABaseItem;
class USlotHUDWidget;

UCLASS()
class MATRIX_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UWeaponHUDWidget* WeaponHUDWidget;

	UPROPERTY(meta = (BindWidget))
	USlotHUDWidget* SlotHUDWidget;

	UFUNCTION(BlueprintCallable)
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION()
	void UpdateWeaponIcon(EWeaponType WeaponType);
	UFUNCTION(BlueprintCallable)
	void UpdateQuickSlots(const TArray<ABaseItem*>& Items);

protected:

};
