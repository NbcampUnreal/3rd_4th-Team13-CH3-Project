#include "UI/Widget/MainHUDWidget.h"
#include "Components/TextBlock.h"

void UMainHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
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