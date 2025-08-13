#include "UI/Widget/MainHUDWidget.h"

void UMainHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (WeaponHUDWidget)
	{
		WeaponHUDWidget->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}

void UMainHUDWidget::UpdateWeaponIcon(EWeaponType WeaponType)
{
	if (WeaponHUDWidget)	
	{
		WeaponHUDWidget->UpdateWeaponIcon(WeaponType);
	}
}