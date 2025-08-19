#include "Systems/MatrixTriggerBox.h"
#include "Characters/MainPlayerCharacter.h"
#include "AI/EnemyCharacter.h"
#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"

// 상수 정의
namespace MatrixTriggerBoxConstants
{
    constexpr float DOOR_SEARCH_RADIUS = 1000.0f;
    constexpr float VALIDATION_TIMER_INTERVAL = 0.5f;
}

AMatrixTriggerBox::AMatrixTriggerBox()
{
    bIsActivated = false;
    bIsUsed = false;
}

void AMatrixTriggerBox::BeginPlay()
{
    Super::BeginPlay();
    
    OnActorBeginOverlap.AddDynamic(this, &AMatrixTriggerBox::OnBeginOverlap);
    OnActorEndOverlap.AddDynamic(this, &AMatrixTriggerBox::OnEndOverlap);
    
    if (TriggerInfo.TriggerType == EMatrixTriggerType::DoorControl)
    {
        InitializeDoorReference();
    }
    
    if (bShowDebug)
    {
        LogTriggerInfo(TEXT("Trigger Box Initialized"));
    }
}

void AMatrixTriggerBox::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!CanActivate() || !IsValidActor(OtherActor)) 
    {
        return;
    }
    
    if (TriggerInfo.TriggerType == EMatrixTriggerType::DoorControl)
    {
        HandleDoorControlOverlap(OtherActor);
        return;
    }
    
    if (TriggerInfo.TriggerCondition == ETriggerCondition::OnEnter)
    {
        ActivateTrigger();
    }
}

void AMatrixTriggerBox::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (TriggerInfo.TriggerType == EMatrixTriggerType::DoorControl)
    {
        return;
    }
    
    if (IsValidActor(OtherActor) && TriggerInfo.TriggerCondition == ETriggerCondition::OnExit)
    {
        DeactivateTrigger();
    }
}

void AMatrixTriggerBox::ActivateTrigger()
{
    bool bShouldBlock = bIsActivated || (bIsUsed && TriggerInfo.bOneTimeUse);
    if (bShouldBlock) 
    {
        return;
    }
    
    bIsActivated = true;
    
    if (bShowDebug)
    {
        LogTriggerInfo(TEXT("Trigger Activated"));
    }
    
    OnTriggerActivated.Broadcast(TriggerInfo);
    
    if (TriggerInfo.Delay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AMatrixTriggerBox::ExecuteDelayedAction, TriggerInfo.Delay, false);
    }
    else
    {
        ExecuteTriggerAction();
    }
}

void AMatrixTriggerBox::DeactivateTrigger()
{
    if (!bIsActivated) return;
    
    bIsActivated = false;
    
    if (bShowDebug)
    {
        LogTriggerInfo(TEXT("Trigger Deactivated"));
    }
    
    OnTriggerDeactivated.Broadcast(TriggerInfo);
}

void AMatrixTriggerBox::ExecuteTriggerAction()
{
    if (bShowDebug)
    {
        LogTriggerInfo(TEXT("Executing Trigger Action"));
    }
    
    switch (TriggerInfo.TriggerType)
    {
    case EMatrixTriggerType::LevelTransition:
        HandleLevelTransition();
        break;
    case EMatrixTriggerType::SubLevelLoad:
        HandleSubLevelLoad();
        break;
    case EMatrixTriggerType::SubLevelUnload:
        HandleSubLevelUnload();
        break;
    case EMatrixTriggerType::WaveStart:
        HandleWaveStart();
        break;
    case EMatrixTriggerType::DoorControl:
        HandleDoorControl();
        break;
    case EMatrixTriggerType::FloorTransition:
        HandleFloorTransition();
        break;
    case EMatrixTriggerType::FloorWaveComplete:
        HandleFloorWaveComplete();
        break;
    case EMatrixTriggerType::Custom:
        HandleCustom();
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("[%s] Unknown trigger type"), *GetName());
        break;
    }
    
    if (TriggerInfo.bOneTimeUse)
    {
        bIsUsed = true;
    }
}

void AMatrixTriggerBox::ExecuteDelayedAction()
{
    ExecuteTriggerAction();
}

void AMatrixTriggerBox::HandleLevelTransition()
{
    if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
    {
        LevelManager->RequestLevelTransition(TriggerInfo.TargetLevel, ELevelTransitionType::NextLevel);
    }
}

void AMatrixTriggerBox::HandleSubLevelLoad()
{
    if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
    {
        LevelManager->LoadSubLevel(TriggerInfo.SubLevelName);
    }
}

void AMatrixTriggerBox::HandleSubLevelUnload()
{
    if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
    {
        LevelManager->UnloadSubLevel(TriggerInfo.SubLevelName);
    }
}

void AMatrixTriggerBox::HandleWaveStart()
{
    if (AMatrixGameMode* GameMode = Cast<AMatrixGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GameMode->StartSpecificWave(TriggerInfo.WaveNumber);
    }
}

void AMatrixTriggerBox::HandleCustom()
{
    UE_LOG(LogTemp, Log, TEXT("Custom trigger executed: %s"), *TriggerInfo.CustomEventName);
}

void AMatrixTriggerBox::HandleDoorControl()
{
    if (!EnsureDoorReference())
    {
        return;
    }

    PlayDoorTimeline();
}

void AMatrixTriggerBox::HandleDoorControlOverlap(AActor* OtherActor)
{
    if (IsEnemyAI(OtherActor) && TriggerInfo.bOpenOnEnter)
    {
        ActivateTrigger();
        StartValidationTimer();
    }
}

bool AMatrixTriggerBox::CanActivate() const
{
    if (TriggerInfo.TriggerType == EMatrixTriggerType::DoorControl)
    {
        return true;
    }
    
    return !bIsUsed || !TriggerInfo.bOneTimeUse;
}

bool AMatrixTriggerBox::IsValidActor(AActor* Actor) const
{
    return IsPlayer(Actor) || IsEnemyAI(Actor);
}

bool AMatrixTriggerBox::IsPlayer(AActor* Actor) const
{
    return Cast<AMainPlayerCharacter>(Actor) != nullptr;
}

bool AMatrixTriggerBox::IsEnemyAI(AActor* Actor) const
{
    return Cast<AEnemyCharacter>(Actor) != nullptr;
}

void AMatrixTriggerBox::LogTriggerInfo(const FString& Action) const
{
    UE_LOG(LogTemp, Log, TEXT("[%s] %s - Type: %s, Condition: %s, Target: %s"), 
        *GetName(), *Action, 
        *UEnum::GetValueAsString(TriggerInfo.TriggerType),
        *UEnum::GetValueAsString(TriggerInfo.TriggerCondition),
        *TriggerInfo.TargetLevel.ToString());
}

bool AMatrixTriggerBox::EnsureDoorReference()
{
    if (!TriggerInfo.TargetDoor)
    {
        TriggerInfo.TargetDoor = FindDoorActor();
    }
    return TriggerInfo.TargetDoor != nullptr;
}

AActor* AMatrixTriggerBox::FindDoorActor() const
{
    if (TriggerInfo.TargetDoor)
    {
        return TriggerInfo.TargetDoor;
    }
    
    // 이름으로 검색
    if (!DoorActorName.IsNone())
    {
        AActor* FoundDoor = FindDoorByName(DoorActorName.ToString());
        if (FoundDoor)
        {
            return FoundDoor;
        }
    }
    
    // 자동 검색
    if (bAutoFindDoor)
    {
        return FindClosestDoor();
    }
    
    return nullptr;
}

AActor* AMatrixTriggerBox::FindDoorByName(const FString& DoorName) const
{
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName().Contains(DoorName))
        {
            return Actor;
        }
    }
    
    return nullptr;
}

AActor* AMatrixTriggerBox::FindClosestDoor() const
{
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    
    AActor* ClosestDoor = nullptr;
    float ClosestDistance = MatrixTriggerBoxConstants::DOOR_SEARCH_RADIUS;
    
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName().Contains(TEXT("Door")))
        {
            float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
            
            if (Distance < ClosestDistance)
            {
                ClosestDoor = Actor;
                ClosestDistance = Distance;
            }
        }
    }
    
    return ClosestDoor;
}

void AMatrixTriggerBox::InitializeDoorReference()
{
    if (!TriggerInfo.TargetDoor)
    {
        TriggerInfo.TargetDoor = FindDoorActor();
    }
}

void AMatrixTriggerBox::StartValidationTimer()
{
    if (TriggerInfo.TriggerType == EMatrixTriggerType::DoorControl)
    {
        if (GetWorld()->GetTimerManager().IsTimerActive(ValidationTimerHandle))
        {
            StopValidationTimer();
        }
        
        GetWorld()->GetTimerManager().SetTimer(ValidationTimerHandle, this, &AMatrixTriggerBox::ValidateActorsInTrigger, 
            MatrixTriggerBoxConstants::VALIDATION_TIMER_INTERVAL, true);
    }
}

void AMatrixTriggerBox::StopValidationTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(ValidationTimerHandle);
}

void AMatrixTriggerBox::ValidateActorsInTrigger()
{
    if (TriggerInfo.TriggerType != EMatrixTriggerType::DoorControl) 
    {
        return;
    }
    
    int32 ActualActorCount = CountValidActorsInTrigger();
    
    if (ActualActorCount == 0 && TriggerInfo.bCloseOnExit)
    {
        CloseDoor();
        StopValidationTimer();
    }
}

int32 AMatrixTriggerBox::CountValidActorsInTrigger() const
{
    int32 Count = 0;
    
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors);
    
    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->IsValidLowLevel() && IsEnemyAI(Actor))
        {
            Count++;
        }
    }
    
    return Count;
}

void AMatrixTriggerBox::PlayDoorTimeline()
{
    if (!TriggerInfo.TargetDoor)
    {
        return;
    }
    
    TArray<UTimelineComponent*> TimelineComponents;
    TriggerInfo.TargetDoor->GetComponents<UTimelineComponent>(TimelineComponents);
    
    for (UTimelineComponent* Timeline : TimelineComponents)
    {
        if (Timeline && Timeline->IsValidLowLevel())
        {
            Timeline->Play();
        }
    }
}

void AMatrixTriggerBox::CloseDoor()
{
    if (!TriggerInfo.TargetDoor)
    {
        return;
    }
    
    TArray<UTimelineComponent*> TimelineComponents;
    TriggerInfo.TargetDoor->GetComponents<UTimelineComponent>(TimelineComponents);
    
    for (UTimelineComponent* Timeline : TimelineComponents)
    {
        if (Timeline && Timeline->IsValidLowLevel())
        {
            Timeline->Reverse();
        }
    }
}

// === 층별 진행 처리 함수들 ===

void AMatrixTriggerBox::HandleFloorTransition()
{
    UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>();
    if (!LevelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager not found for floor transition"));
        return;
    }
    
    // 웨이브 완료 체크가 활성화되어 있으면 실제 웨이브 진행 상태 확인
    if (TriggerInfo.bCheckWaveCompletion)
    {
        if (!LevelManager->CanProceedToNextFloorWithWaveCheck())
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot proceed to next floor. Current floor waves not completed or enemies still remaining."));
            return;
        }
    }
    
    // 다음 층으로 진행
    LevelManager->ProceedToNextFloor();
    UE_LOG(LogTemp, Log, TEXT("Floor transition triggered. Moving to floor %d"), LevelManager->GetCurrentFloor());
}

void AMatrixTriggerBox::HandleFloorWaveComplete()
{
    UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>();
    if (!LevelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager not found for floor wave complete"));
        return;
    }
    
    int32 CurrentFloor = LevelManager->GetCurrentFloor();
    LevelManager->SetFloorWaveCompleted(CurrentFloor, true);
    UE_LOG(LogTemp, Log, TEXT("Floor %d wave completion triggered"), CurrentFloor);
}