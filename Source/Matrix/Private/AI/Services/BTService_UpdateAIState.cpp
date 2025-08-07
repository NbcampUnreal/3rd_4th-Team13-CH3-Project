
#include "AI/Services/BTService_UpdateAIState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTService_UpdateAIState::UBTService_UpdateAIState()
{
	NodeName = TEXT("Update AI State");
	Interval = 0.5f; // 0.5초마다 업데이트
	RandomDeviation = 0.1f;

	TargetActorKey.SelectedKeyName = "TargetActor";
	DistanceToTargetKey.SelectedKeyName = "DistanceToTarget";
	IsInAttackRangeKey.SelectedKeyName = "IsInAttackRange";
}

void UBTService_UpdateAIState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	APawn* AIPawn = AIController->GetPawn();

	if (TargetActor && AIPawn)
	{
		float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
		BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
		BlackboardComp->SetValueAsBool(IsInAttackRangeKey.SelectedKeyName, Distance <= AttackRange);
	}
	else
	{
		BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, FLT_MAX);
		BlackboardComp->SetValueAsBool(IsInAttackRangeKey.SelectedKeyName, false);
	}
}
