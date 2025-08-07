#include "UI/Widget/WeaponHUDWidget.h"
#include "Components/Image.h"

void UWeaponHUDWidget::UpdateWeaponIcon(EWeaponType WeaponType)
{
	if (!WeaponImage) return;

	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
		WeaponImage->SetBrushFromTexture(PistolIcon);
		break;
	case EWeaponType::Rifle:
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
		WeaponImage->SetBrushFromTexture(RifleIcon);
		break;
	case EWeaponType::Shotgun:
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
		WeaponImage->SetBrushFromTexture(ShotgunIcon);
		break;
	default:
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
		WeaponImage->SetBrushFromTexture(NoneIcon);
		break;
	}
}
