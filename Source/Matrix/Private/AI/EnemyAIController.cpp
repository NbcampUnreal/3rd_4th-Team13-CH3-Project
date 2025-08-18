#include "AI/EnemyAIController.h"
#include "AI/EnemyCharacter.h"
#include "AI/AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AI/Services/PlayerLocationSharingService.h"
#include "Characters/MainPlayerCharacter.h"
#include "Perception/AISense.h"

// Define the actual variables for the blackboard keys
namespace BlackboardKeys
{
    const FName TargetActorKey = TEXT("TargetActor");
    const FName LastKnownPlayerLocationKey = TEXT("LastKnownPlayerLocation");
    const FName HasLineOfSightKey = TEXT("HasLineOfSight");
    const FName IsUnderAttackKey = TEXT("IsUnderAttack");
    const FName InvestigationLocationKey = TEXT("InvestigationLocation");
    const FName IsPlayerDeadKey = TEXT("IsPlayerDead");
}

AEnemyAIController::AEnemyAIController()
{
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));

    AttackRange = 800.0f;
    MoveRadius = 1000.0f;
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    if (AIPerception)
    {
        AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
    }
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (InPawn != nullptr && BehaviorTreeAsset != nullptr)
    {
        BlackboardComp->InitializeBlackboard(*(BehaviorTreeAsset->BlackboardAsset));
        RunBehaviorTree(BehaviorTreeAsset);

        // Set initial player location for immediate pursuit
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            if (PlayerPawn)
        {
            BlackboardComp->SetValueAsVector(BlackboardKeys::LastKnownPlayerLocationKey, PlayerPawn->GetActorLocation());
            // Also set InvestigationLocation to player's initial spawn location for initial pursuit
            BlackboardComp->SetValueAsVector(BlackboardKeys::InvestigationLocationKey, PlayerPawn->GetActorLocation());

            // Cast to MainPlayerCharacter and subscribe to OnPlayerDeath delegate
            AMainPlayerCharacter* MainPlayer = Cast<AMainPlayerCharacter>(PlayerPawn);
            if (MainPlayer)
            {
                MainPlayer->OnPlayerDeath.AddUObject(this, &AEnemyAIController::OnPlayerDied);
                UE_LOG(LogTemp, Warning, TEXT("Subscribed to player death delegate."));
            }
        }

            // Cast to MainPlayerCharacter and subscribe to OnPlayerDeath delegate
            AMainPlayerCharacter* MainPlayer = Cast<AMainPlayerCharacter>(PlayerPawn);
            if (MainPlayer)
            {
                MainPlayer->OnPlayerDeath.AddUObject(this, &AEnemyAIController::OnPlayerDied);
                UE_LOG(LogTemp, Warning, TEXT("Subscribed to player death delegate."));
            }
        }
    }
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Actor->IsA(AEnemyCharacter::StaticClass()))
    {
        return;
    }

    if (Stimulus.Type == SightConfig->GetSenseID())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            BlackboardComp->SetValueAsObject(BlackboardKeys::TargetActorKey, Actor);
            BlackboardComp->SetValueAsBool(BlackboardKeys::HasLineOfSightKey, true);
            BlackboardComp->SetValueAsVector(BlackboardKeys::LastKnownPlayerLocationKey, Actor->GetActorLocation());
            SetFocus(Actor);

            // --- NEW --- Update the global location sharing service
            if (UPlayerLocationSharingService* LocationService = UPlayerLocationSharingService::GetInstance())
            {
                LocationService->UpdatePlayerLocation(Actor->GetActorLocation());
            }
        }
        else // Player is lost
        {
            BlackboardComp->SetValueAsBool(BlackboardKeys::HasLineOfSightKey, false);
            ClearFocus(EAIFocusPriority::Gameplay);

            // Set InvestigationLocation to AI's current location when player is lost
            if (APawn* MyPawn = GetPawn())
            {
                BlackboardComp->SetValueAsVector(BlackboardKeys::InvestigationLocationKey, MyPawn->GetActorLocation());
            }
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

void AEnemyAIController::OnPlayerDied()
{
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(BlackboardKeys::IsPlayerDeadKey, true);
        UE_LOG(LogTemp, Warning, TEXT("Player has died. Setting IsPlayerDeadKey to true."));
    }
}
