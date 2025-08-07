#include "AI/Tasks/BTTask_TakeCover.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_TakeCover::UBTTask_TakeCover()
{
	NodeName = TEXT("Take Cover");
}

EBTNodeResult::Type UBTTask_TakeCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return EBTNodeResult::Failed;

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface) return EBTNodeResult::Failed;

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return EBTNodeResult::Failed;

	if (TakeCoverAbilityClass)
	{
		if (AbilitySystemComponent->TryActivateAbilityByClass(TakeCoverAbilityClass))
		{
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
