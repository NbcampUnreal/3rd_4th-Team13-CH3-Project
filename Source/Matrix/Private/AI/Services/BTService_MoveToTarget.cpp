#include "AI/Services/BTService_MoveToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AIBlackboardKeys.h"
#include "AI/EnemyAIController.h"
#include "AI/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_MoveToTarget::UBTService_MoveToTarget()
{
	NodeName = TEXT("Move To Target (Range)");

	// Set this service to tick every frame
	Interval = 0.1f;
	RandomDeviation = 0.0f;

	// Default blackboard key for target actor
	TargetActorKey.SelectedKeyName = BlackboardKeys::TargetActorKey;
}

void UBTService_MoveToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (!BlackboardComp || !AIController)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	bool bHasLineOfSight = BlackboardComp->GetValueAsBool(BlackboardKeys::HasLineOfSightKey);

	if (!TargetActor || !bHasLineOfSight)
	{
		// Stop movement if no target or no line of sight
		AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AIController->GetPawn());
		if (AIChar && AIChar->GetCharacterMovement())
		{
			AIChar->GetCharacterMovement()->StopMovementImmediately();
		}
		return;
	}

	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AIController->GetPawn());
	if (!AIChar || !AIChar->GetCharacterMovement())
	{
		return;
	}

	FVector CurrentLocation = AIChar->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	// Set movement speed
	AIChar->SetMovementSpeed(MovementSpeed);

	// If outside max distance, move towards target
	if (Distance > MaxDistance)
	{
		AIController->MoveToActor(TargetActor, AcceptanceRadius);
	}
	// If inside min distance, move away from target (simple evasion/kiting)
	else if (Distance < MinDistance)
	{
		FVector DirectionAway = (CurrentLocation - TargetLocation).GetSafeNormal();
		FVector MoveAwayLocation = CurrentLocation + DirectionAway * (MinDistance - Distance + AcceptanceRadius);
		AIController->MoveToLocation(MoveAwayLocation, AcceptanceRadius);
	}
	// If within desired range, stop movement
	else
	{
		AIChar->GetCharacterMovement()->StopMovementImmediately();
	}
}
