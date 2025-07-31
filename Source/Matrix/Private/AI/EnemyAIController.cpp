#include "AI/EnemyAIController.h"

#include "NavigationSystem.h"
#include "TimerManager.h"
#include "AI/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);

		// StartBehaviorTree();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Blackboard Component not found"));
	}
	
	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&AEnemyAIController::OnPerceptionUpdated
		);
	}
}

// void AEnemyAIController::StartBehaviorTree()
// {
// 	if (BehaviorTreeAsset)
// 	{
// 		RunBehaviorTree(BehaviorTreeAsset);
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Behavior Tree Asset not set!"));
// 	}
// }

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Sparta] AI Controller is controlling %s."), *InPawn->GetName());
	}
}

void AEnemyAIController::MoveToRandomLocation()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[Sparta] No Pawn to control."));
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[Sparta] Could not find Navigation System."));
	}

	FNavLocation RandomLocation;
	bool bFoundLocation = NavSystem->GetRandomReachablePointInRadius(
		MyPawn->GetActorLocation(),
		MoveRadius,
		RandomLocation
	);

	if (bFoundLocation)
	{
		MoveToLocation(RandomLocation.Location);

		UE_LOG(LogTemp, Warning, TEXT("[Sparta] Move target: %s"), *RandomLocation.Location.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Sparta] Could not find a reachable location."));
	}
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Actor != PlayerPawn)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Sparta] Saw something! %s"), *Actor->GetName());

		DrawDebugString(
			GetWorld(),
			Actor->GetActorLocation() + FVector(0, 0, 100),
			FString::Printf(TEXT("Saw: %s"), *Actor->GetName()),
			nullptr,
			FColor::Green,
			2.0f,
			true
		);
		
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true);
		BlackboardComp->SetValueAsVector(TEXT("TargetLastKnownLocation"), Actor->GetActorLocation());
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
		
		StartChasing(Actor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Sparta] Missed it! %s"), *Actor->GetName());

		DrawDebugString(
			GetWorld(),
			Actor->GetActorLocation() + FVector(0, 0, 100),
			FString::Printf(TEXT("Missed: %s"), *Actor->GetName()),
			nullptr,
			FColor::Red,
			2.0f,
			true
		);

		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), true);
		
		StopChasing();
	}
}

void AEnemyAIController::StartChasing(AActor* Target)
{
	if (bIsChasing && CurrentTarget == Target) return;

	CurrentTarget = Target;
	bIsChasing = true;

	GetWorldTimerManager().ClearTimer(RandomMoveTimer);

	if (AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(GetPawn()))
	{
		AIChar->SetMovementSpeed(AIChar->RunSpeed);
	}

	UpdateChase();
	GetWorldTimerManager().SetTimer(
		ChaseTimer,
		this,
		&AEnemyAIController::UpdateChase,
		0.25f,
		true
	);
}

void AEnemyAIController::StopChasing()
{
	if (!bIsChasing) return;

	CurrentTarget = nullptr;
	bIsChasing = false;

	GetWorldTimerManager().ClearTimer(ChaseTimer);

	StopMovement();

	if (AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(GetPawn()))
	{
		AIChar->SetMovementSpeed(AIChar->WalkSpeed);
	}

	GetWorldTimerManager().SetTimer(
		RandomMoveTimer,
		this,
		&AEnemyAIController::MoveToRandomLocation,
		3.0f,
		true,
		2.0f
	);
}

void AEnemyAIController::UpdateChase()
{
	if (CurrentTarget && bIsChasing)
	{
		MoveToActor(CurrentTarget, 100.0f);
	}
}