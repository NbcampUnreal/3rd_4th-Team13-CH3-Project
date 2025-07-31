#include "AI/BTTask_FindRandomLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");

	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, LocationKey));
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;

	FNavLocation RandomLocation;
	bool bFound = NavSystem->GetRandomReachablePointInRadius(
		MyPawn->GetActorLocation(),
		SearchRadius,
		RandomLocation
	);

	if (bFound)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector(LocationKey.SelectedKeyName, RandomLocation.Location);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
