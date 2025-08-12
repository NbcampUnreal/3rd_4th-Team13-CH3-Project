#include "AI/Tasks/BTTask_Evade.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_Evade::UBTTask_Evade()
{
    NodeName = TEXT("Evade");
}

EBTNodeResult::Type UBTTask_Evade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController || !AIController->GetPawn())
    {
        return EBTNodeResult::Failed;
    }

    IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
    if (!AbilitySystemInterface)
    {
        return EBTNodeResult::Failed;
    }

    UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
    if (!AbilitySystemComponent)
    {
        return EBTNodeResult::Failed;
    }

    if (EvadeAbilityClass)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(EvadeAbilityClass))
        {
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}

