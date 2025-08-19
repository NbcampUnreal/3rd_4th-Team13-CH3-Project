#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/WeaponHUDWidget.h"
#include "UI/Widget/MissionHUDWidget.h"
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
	
	UPROPERTY(meta = (BindWidget))
	UMissionHUDWidget* MissionHUDWidget;

	UFUNCTION(BlueprintCallable)
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION(BlueprintCallable)
	void UpdateWeaponIcon(EWeaponType WeaponType);
	UFUNCTION(BlueprintCallable)
	void UpdateQuickSlots(const TArray<ABaseItem*>& Items);
	UFUNCTION(BlueprintCallable)
	void UpdateKillCount(int32 KillCount);

protected:

};
