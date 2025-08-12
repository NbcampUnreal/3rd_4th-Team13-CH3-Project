#include "AI/EnemyAIController.h"
#include "AI/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h" // Added for damage sense
#include "Perception/AISense_Damage.h" // Added for UAISense_Damage
#include "Perception/AISense.h" // Added for UAISense::GetSenseID

AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(1.0f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	// Configure Damage Sense
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	AIPerception->ConfigureSense(*DamageConfig);

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));

	AttackRange = 800.0f;
	MoveRadius = 1000.0f;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Blackboard Component not found"));
	}
	
	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Sparta] AI Controller is controlling %s."), *InPawn->GetName());
		if (BlackboardComp)
		{
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (PlayerPawn)
			{
				BlackboardComp->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
				BlackboardComp->SetValueAsVector(TEXT("TargetLastKnownLocation"), PlayerPawn->GetActorLocation());
				BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true); // Assume seen if known
			}
		}
		if (BehaviorTreeAsset)
		{
			RunBehaviorTree(BehaviorTreeAsset);
		}
	}
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Actor != PlayerPawn) return;

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Sparta] Saw target: %s"), *Actor->GetName());
			BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
			BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true);
			BlackboardComp->SetValueAsVector(TEXT("TargetLastKnownLocation"), Actor->GetActorLocation());
			BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Sparta] Lost target: %s"), *Actor->GetName());
			BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
			BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), true);
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		if (Stimulus.WasSuccessfullySensed()) // Damage sense always successfully sensed if triggered
		{
			UE_LOG(LogTemp, Warning, TEXT("[Sparta] Received damage from: %s"), *Actor->GetName());
			BlackboardComp->SetValueAsBool(TEXT("PlayerAttacked"), true); // Set Blackboard key
			// You might want to clear this key after a short delay or after evade/cover
		}
	}
}

void AEnemyAIController::PerformAttack()
{
	AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(GetPawn());
	if (AIChar)
	{
		AIChar->FireProjectile();
	}
}
