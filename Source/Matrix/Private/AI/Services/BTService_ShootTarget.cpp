#include "AI/Services/BTService_ShootTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AIBlackboardKeys.h"
#include "AI/EnemyAIController.h"
#include "AI/EnemyCharacter.h"
#include "AbilitySystemComponent.h"

UBTService_ShootTarget::UBTService_ShootTarget()
{
	NodeName = TEXT("Shoot Target");

	// Set the key selector to use the correct blackboard key by default
	HasLineOfSightKey.SelectedKeyName = BlackboardKeys::HasLineOfSightKey;

	// We want this service to tick every frame to check for attack opportunities
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_ShootTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (!BlackboardComp || !AIController)
	{
		return;
	}

	// Only shoot if we have a line of sight and player is not dead
	bool bHasLineOfSight = BlackboardComp->GetValueAsBool(HasLineOfSightKey.SelectedKeyName);
	bool bIsPlayerDead = BlackboardComp->GetValueAsBool(BlackboardKeys::IsPlayerDeadKey);

	if (bHasLineOfSight && !bIsPlayerDead)
	{
		AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(AIController->GetPawn());
		if (AIChar)
		{
			UAbilitySystemComponent* ASC = AIChar->GetAbilitySystemComponent();
			TSubclassOf<UGameplayAbility> AttackAbility = AIChar->GetAttackAbilityClass();

			if (ASC && AttackAbility)
			{
				ASC->TryActivateAbilityByClass(AttackAbility);
			}
		}
	}
}
