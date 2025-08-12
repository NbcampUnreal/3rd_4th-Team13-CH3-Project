#include "Systems/MatrixTriggerBox.h"
#include "Characters/MainPlayerCharacter.h"
#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixLevelManager.h"
#include "Kismet/GameplayStatics.h"

AMatrixTriggerBox::AMatrixTriggerBox()
{
    // 기본 설정
    bIsActivated = false;
    bIsUsed = false;
}

void AMatrixTriggerBox::BeginPlay()
{
    Super::BeginPlay();
    
    // 오버랩 이벤트 바인딩
    OnActorBeginOverlap.AddDynamic(this, &AMatrixTriggerBox::OnBeginOverlap);
    OnActorEndOverlap.AddDynamic(this, &AMatrixTriggerBox::OnEndOverlap);
    
    if (bShowDebug)
    {
        LogTriggerInfo(TEXT("Trigger Box Initialized"));
    }
}

void AMatrixTriggerBox::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!CanActivate() || !IsPlayer(OtherActor)) return;
    
    if (TriggerInfo.TriggerCondition == ETriggerCondition::OnEnter)
    {
        ActivateTrigger();
    }
}

void AMatrixTriggerBox::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!IsPlayer(OtherActor)) return;
    
    if (TriggerInfo.TriggerCondition == ETriggerCondition::OnExit)
    {
        DeactivateTrigger();
    }
}

void AMatrixTriggerBox::ActivateTrigger()
{
    if (bIsActivated || (bIsUsed && TriggerInfo.bOneTimeUse)) return;
    
    bIsActivated = true;
    
    if (bShowDebug)
    {
        LogTriggerInfo(TEXT("Trigger Activated"));
    }
    
    // 이벤트 브로드캐스트
    OnTriggerActivated.Broadcast(TriggerInfo);
    
    // 지연 실행이 있다면 타이머 설정
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
    
    // 이벤트 브로드캐스트
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
    case EMatrixTriggerType::DoorOpen:
        HandleDoorOpen();
        break;
    case EMatrixTriggerType::Custom:
        HandleCustom();
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown trigger type"));
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

void AMatrixTriggerBox::HandleDoorOpen()
{
    // 문 열기 시스템은 향후 구현 예정
    UE_LOG(LogTemp, Log, TEXT("Door Open triggered: %s"), *TriggerInfo.CustomEventName);
}

void AMatrixTriggerBox::HandleCustom()
{
    UE_LOG(LogTemp, Log, TEXT("Custom trigger executed: %s"), *TriggerInfo.CustomEventName);
}

bool AMatrixTriggerBox::CanActivate() const
{
    return !bIsUsed || !TriggerInfo.bOneTimeUse;
}

bool AMatrixTriggerBox::IsPlayer(AActor* Actor) const
{
    return Cast<AMainPlayerCharacter>(Actor) != nullptr;
}

void AMatrixTriggerBox::LogTriggerInfo(const FString& Action) const
{
    UE_LOG(LogTemp, Log, TEXT("[%s] %s - Type: %s, Condition: %s, Target: %s"), 
        *GetName(), *Action, 
        *UEnum::GetValueAsString(TriggerInfo.TriggerType),
        *UEnum::GetValueAsString(TriggerInfo.TriggerCondition),
        *TriggerInfo.TargetLevel.ToString());
}
