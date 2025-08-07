#include "AI/Tasks/BTTask_Evade.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_Evade::UBTTask_Evade()
{
	NodeName = TEXT("Evade");
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Evade, TargetLocationKey));
}

EBTNodeResult::Type UBTTask_Evade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return EBTNodeResult::Failed;

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface) return EBTNodeResult::Failed;

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return EBTNodeResult::Failed;

	if (EvadeAbilityClass)
	{
		if (AbilitySystemComponent->TryActivateAbilityByClass(EvadeAbilityClass))
		{
			APawn* AIPawn = AIController->GetPawn();
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (AIPawn && PlayerPawn)
			{
				FVector CurrentLocation = AIPawn->GetActorLocation();
				FVector PlayerLocation = PlayerPawn->GetActorLocation();
				FVector DirectionAwayFromPlayer = (CurrentLocation - PlayerLocation).GetSafeNormal();
				FVector EvadeLocation = CurrentLocation + DirectionAwayFromPlayer * EvadeDistance;

				UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (NavSys)
				{
					FNavLocation NavLocation;
					if (NavSys->GetRandomPointInNavigableRadius(EvadeLocation, EvadeDistance, NavLocation))
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, NavLocation.Location);
					}
				}
			}
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
