
#include "AI/Tasks/BTTask_Attack.h"
#include "AIController.h"
#include "AI/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Target");
	TargetActorKey.SelectedKeyName = "TargetActor";
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return EBTNodeResult::Failed;

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface) return EBTNodeResult::Failed;

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return EBTNodeResult::Failed;

	if (AttackAbilityClass)
	{
		if (AbilitySystemComponent->TryActivateAbilityByClass(AttackAbilityClass))
		{
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
