#include "AI/Services/BTService_UpdateFromSharedLocation.h"
#include "AI/Services/PlayerLocationSharingService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AIBlackboardKeys.h"

UBTService_UpdateFromSharedLocation::UBTService_UpdateFromSharedLocation()
{
	NodeName = TEXT("Update Location From Shared Service");

	// Set the key selector to use the correct blackboard key by default
	LastKnownPlayerLocationKey.SelectedKeyName = BlackboardKeys::LastKnownPlayerLocationKey;
}

void UBTService_UpdateFromSharedLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	// Don't update if we have direct line of sight
	bool bHasLineOfSight = BlackboardComp->GetValueAsBool(BlackboardKeys::HasLineOfSightKey);
	if (bHasLineOfSight)
	{
		return;
	}

	UPlayerLocationSharingService* LocationService = UPlayerLocationSharingService::GetInstance();
	if (LocationService && LocationService->HasValidLocation())
	{
		FVector SharedLocation;
		if (LocationService->GetLastKnownPlayerLocation(SharedLocation))
		{
			BlackboardComp->SetValueAsVector(LastKnownPlayerLocationKey.SelectedKeyName, SharedLocation);
		}
	}
}
