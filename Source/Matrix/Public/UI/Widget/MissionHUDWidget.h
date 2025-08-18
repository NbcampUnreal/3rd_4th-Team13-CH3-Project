#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionHUDWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class MATRIX_API UMissionHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateKillCount(int32 KillCount);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_KillCount;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PunchKillCountScale;
	
};
