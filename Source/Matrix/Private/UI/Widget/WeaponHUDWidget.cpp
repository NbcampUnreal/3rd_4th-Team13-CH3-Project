#include "UI/Widget/WeaponHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWeaponHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!CurrentAmmoText || !MaxAmmoText) return;

	CurrentAmmoText->SetText(FText::AsNumber(CurrentAmmo));
	MaxAmmoText->SetText(FText::AsNumber(MaxAmmo));

	const float AmmoRatio = MaxAmmo > 0 ? (float)CurrentAmmo / (float)MaxAmmo : 1.0f;

	if (AmmoRatio <= 0.4f)
	{
		CurrentAmmoText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		if (PunchAmmoScale)
		{
			PlayAnimation(PunchAmmoScale, 0.f, 1);
		}


	}
	else
	{
		CurrentAmmoText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}

}

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
