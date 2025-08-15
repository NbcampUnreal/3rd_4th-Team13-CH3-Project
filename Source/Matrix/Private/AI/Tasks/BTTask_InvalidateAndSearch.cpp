#include "AI/Tasks/BTTask_InvalidateAndSearch.h"
#include "AI/Services/PlayerLocationSharingService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AIBlackboardKeys.h"

UBTTask_InvalidateAndSearch::UBTTask_InvalidateAndSearch()
{
	NodeName = TEXT("Invalidate Shared Location and Clear Key");

	// Set the key selector to use the correct blackboard key by default
	LastKnownPlayerLocationKey.SelectedKeyName = BlackboardKeys::LastKnownPlayerLocationKey;
}

EBTNodeResult::Type UBTTask_InvalidateAndSearch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	UPlayerLocationSharingService* LocationService = UPlayerLocationSharingService::GetInstance();
	if (LocationService)
	{
		// Get the location from our blackboard
		const FVector LocationToInvalidate = BlackboardComp->GetValueAsVector(LastKnownPlayerLocationKey.SelectedKeyName);

		// Tell the service that this location is stale
		LocationService->InvalidateLocation(LocationToInvalidate);
	}

	// Clear our own blackboard key so we don't try to go there again
	BlackboardComp->ClearValue(LastKnownPlayerLocationKey.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}
