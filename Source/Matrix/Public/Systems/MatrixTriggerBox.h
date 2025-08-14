#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Core/MatrixCoreTypes.h"
#include "MatrixTriggerBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerActivated, const FTriggerInfo&, TriggerInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerDeactivated, const FTriggerInfo&, TriggerInfo);

UCLASS()
class MATRIX_API AMatrixTriggerBox : public ATriggerBox
{
    GENERATED_BODY()

public:
    AMatrixTriggerBox();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
    FTriggerInfo TriggerInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
    bool bShowDebug = false;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTriggerActivated OnTriggerActivated;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTriggerDeactivated OnTriggerDeactivated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Control")
    FName DoorActorName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Control")
    bool bAutoFindDoor = true;

private:
    bool bIsActivated = false;
    bool bIsUsed = false;
    FTimerHandle DelayTimerHandle;
    FTimerHandle ValidationTimerHandle;

    UFUNCTION()
    void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

    void ActivateTrigger();
    void DeactivateTrigger();
    void ExecuteTriggerAction();
    void ExecuteDelayedAction();

    void HandleLevelTransition();
    void HandleSubLevelLoad();
    void HandleSubLevelUnload();
    void HandleWaveStart();
    void HandleDoorControl();
    void HandleCustom();

    bool CanActivate() const;
    bool IsValidActor(AActor* Actor) const;
    bool IsPlayer(AActor* Actor) const;
    bool IsEnemyAI(AActor* Actor) const;
    void LogTriggerInfo(const FString& Action) const;
    
    // 문 관련 함수들
    bool EnsureDoorReference();
    AActor* FindDoorActor() const;
    AActor* FindDoorByName(const FString& DoorName) const;
    AActor* FindClosestDoor() const;
    void InitializeDoorReference();
    void PlayDoorTimeline();
    void CloseDoor();
    
    // Door Control 전용 함수들
    void HandleDoorControlOverlap(AActor* OtherActor);
    
    // Timer 관련 함수들
    void StartValidationTimer();
    void StopValidationTimer();
    void ValidateActorsInTrigger();
    int32 CountValidActorsInTrigger() const;
};
