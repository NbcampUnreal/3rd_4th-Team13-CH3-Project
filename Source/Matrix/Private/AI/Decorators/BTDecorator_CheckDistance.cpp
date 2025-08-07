#include "AI/Decorators/BTDecorator_CheckDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

UBTDecorator_CheckDistance::UBTDecorator_CheckDistance()
{
	NodeName = TEXT("Check Distance");
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistance, TargetLocationKey));
}

bool UBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return false;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return false;
	
	FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	
	float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetLocation);
	
	if (bCheckWithinRange)
	{
		return Distance <= DistanceThreshold;
	}
	else
	{
		return Distance > DistanceThreshold;
	}
}
