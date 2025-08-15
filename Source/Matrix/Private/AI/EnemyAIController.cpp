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
#include "Weapons/WeaponSystem/BulletBase.h"
#include "TimerManager.h"

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
    UE_LOG(LogTemp, Warning, TEXT("OnPossess CALLED for %s."), *InPawn->GetName());

    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("OnPossess: BehaviorTreeAsset is NULL!"));
        return;
    }

    if (!BehaviorTreeAsset->BlackboardAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("OnPossess: BehaviorTreeAsset->BlackboardAsset is NULL! Assign a Blackboard asset to the Behavior Tree."));
        return;
    }

    // This is the key check
    bool bSuccess = UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp);
    if (bSuccess && BlackboardComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnPossess: UseBlackboard SUCCEEDED. BlackboardComp is now VALID."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnPossess: UseBlackboard FAILED or BlackboardComp is NULL!"));
        return;
    }
    
	RunBehaviorTree(BehaviorTreeAsset);

	if (BrainComponent)
	{
		BrainComponent->StopLogic("Waiting for player to move");
	}
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        if (Cast<ABulletBase>(Actor))
        {
            OnProjectileDetected();
        }
        else if (Actor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
        {
            OnPlayerSeen(Actor);
        }
    }
    else // Stimulus was lost
    {
        if (Actor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
        {
            OnPlayerLost();
        }
    }
}

void AEnemyAIController::ExecuteTurn(float PlayerMovementDistance)
{
    if (BrainComponent && BrainComponent->IsPaused())
    { 
        BrainComponent->StartLogic();
    }
}
