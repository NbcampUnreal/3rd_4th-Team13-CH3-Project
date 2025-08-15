#include "AI/Services/BTService_DetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Detect Player");

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DetectPlayer, TargetActorKey), APawn::StaticClass());
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DetectPlayer, TargetLocationKey));
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	UAIPerceptionComponent* AIPerception = AIController->FindComponentByClass<UAIPerceptionComponent>();
	if (!AIPerception) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	TArray<AActor*> PerceivedActors;
	AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	bool bPlayerPerceived = false;
	for (AActor* PerceivedActor : PerceivedActors)
	{
		if (PerceivedActor == PlayerPawn)
		{
			bPlayerPerceived = true;
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerPawn);
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, PlayerPawn->GetActorLocation());
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeeTarget"), true);
			break;
		}
	}

	if (!bPlayerPerceived)
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetActorKey.SelectedKeyName);
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetLocationKey.SelectedKeyName);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeeTarget"), false);
	}
}
