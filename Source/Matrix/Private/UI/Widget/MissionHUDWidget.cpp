#include "UI/Widget/MissionHUDWidget.h"
#include "Components/TextBlock.h"

void UMissionHUDWidget::UpdateKillCount(int32 KillCount)
{
	if (!Text_KillCount) return;
	
	Text_KillCount->SetText(FText::AsNumber(KillCount));
	
	if (PunchKillCountScale)
	{
		PlayAnimation(PunchKillCountScale, 0.f, 1);
	}
}
