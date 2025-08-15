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
    Super::ExecuteTask(OwnerComp, NodeMemory);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // Get the location to invalidate
    const FVector OldLocation = BlackboardComp->GetValueAsVector(LastKnownPlayerLocationKey.SelectedKeyName);

    // Invalidate the shared location
    if (UPlayerLocationSharingService* LocationService = UPlayerLocationSharingService::GetInstance())
    {
        LocationService->InvalidateLocation(OldLocation);
    }

    // Clear the LastKnownPlayerLocationKey
    BlackboardComp->ClearValue(LastKnownPlayerLocationKey.SelectedKeyName);

    return EBTNodeResult::Succeeded;
}
