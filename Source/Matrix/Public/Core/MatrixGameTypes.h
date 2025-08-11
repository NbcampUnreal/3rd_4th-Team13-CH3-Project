#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	None,
	MainMenu,
	Loading,
	Playing,
	Paused,
	GameOver,
	GameClear
};