#include "AI/EnemyCharacter.h"

#include "AI/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = WalkSpeed;
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	Movement->AirControl = 0.2f;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyCharacter::SetMovementSpeed(float NewSpeed)
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = NewSpeed;
	}
}
