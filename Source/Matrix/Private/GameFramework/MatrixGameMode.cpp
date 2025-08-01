// MatrixGameMode.cpp

#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AI/EnemyCharacter.h"
#include "Characters/MainPlayerCharacter.h"
#include "Characters/MainPlayerController.h"
#include "Engine/TargetPoint.h"

AMatrixGameMode::AMatrixGameMode()
{
	DefaultPawnClass = AMainPlayerCharacter::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass();
	GameStateClass = AMatrixGameState::StaticClass();
}