#include "AI/Tasks/BTTask_EngageTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AIBlackboardKeys.h"

UBTTask_EngageTarget::UBTTask_EngageTarget()
{
	NodeName = TEXT("Engage Target (Latent)");

	// This task will run as long as the condition is met
	bNotifyTick = true;

	// Set the key selector to use the correct blackboard key by default
	HasLineOfSightKey.SelectedKeyName = BlackboardKeys::HasLineOfSightKey;
}

EBTNodeResult::Type UBTTask_EngageTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	bool bHasLineOfSight = BlackboardComp->GetValueAsBool(HasLineOfSightKey.SelectedKeyName);
	if (bHasLineOfSight)
	{
		// We have sight, so we start engaging. Return InProgress to keep this task active.
		return EBTNodeResult::InProgress;
	}

	// We don't have sight, so we can't engage. Fail immediately.
	return EBTNodeResult::Failed;
}

void UBTTask_EngageTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	bool bHasLineOfSight = BlackboardComp->GetValueAsBool(HasLineOfSightKey.SelectedKeyName);
	if (!bHasLineOfSight)
	{
		// We lost sight of the target, so the engagement is over. 
		// Finish the task with success, allowing the BT to move to the next branch.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	// If we still have line of sight, do nothing and the task will remain InProgress.
}
