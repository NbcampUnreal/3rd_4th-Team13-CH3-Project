#include "UI/Widget/MainHUDWidget.h"

void UMainHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!AmmoText) return;

	FString Display = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
	AmmoText->SetText(FText::FromString(Display));
}