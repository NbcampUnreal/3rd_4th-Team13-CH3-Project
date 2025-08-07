#include "AI/Tasks/BTTask_FindCover.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_FindCover::UBTTask_FindCover()
{
	NodeName = TEXT("Find Cover");
	CoverLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindCover, CoverLocationKey));
}

EBTNodeResult::Type UBTTask_FindCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return EBTNodeResult::Failed;

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface) return EBTNodeResult::Failed;

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return EBTNodeResult::Failed;

	if (FindCoverAbilityClass)
	{
		if (AbilitySystemComponent->TryActivateAbilityByClass(FindCoverAbilityClass))
		{
			APawn* AIPawn = AIController->GetPawn();
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (AIPawn && PlayerPawn)
			{
				UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (NavSys)
				{
					FNavLocation RandomNavLocation;
					if (NavSys->GetRandomPointInNavigableRadius(AIPawn->GetActorLocation(), CoverSearchRadius, RandomNavLocation))
					{
						FVector DirectionToPlayer = (PlayerPawn->GetActorLocation() - RandomNavLocation.Location).GetSafeNormal();
						FHitResult HitResult;
						FCollisionQueryParams Params;
						Params.AddIgnoredActor(AIPawn);
						Params.AddIgnoredActor(PlayerPawn);

						if (GetWorld()->LineTraceSingleByChannel(HitResult, RandomNavLocation.Location, PlayerPawn->GetActorLocation(), ECC_Visibility, Params))
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsVector(CoverLocationKey.SelectedKeyName, RandomNavLocation.Location);
						}
					}
				}
			}
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
