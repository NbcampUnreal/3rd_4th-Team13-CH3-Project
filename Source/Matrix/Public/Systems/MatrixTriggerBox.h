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

    // === 트리거 설정 ===
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
    FTriggerInfo TriggerInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
    bool bShowDebug = false;

    // === 이벤트 델리게이트 ===
    
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTriggerActivated OnTriggerActivated;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTriggerDeactivated OnTriggerDeactivated;

private:
    // === 내부 상태 ===
    
    bool bIsActivated = false;
    bool bIsUsed = false;
    FTimerHandle DelayTimerHandle;

    // === 오버랩 이벤트 핸들러 ===
    
    UFUNCTION()
    void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

    // === 트리거 처리 함수들 ===
    
    void ActivateTrigger();
    void DeactivateTrigger();
    void ExecuteTriggerAction();
    void ExecuteDelayedAction();

    // === 트리거 타입별 처리 함수들 ===
    
    void HandleLevelTransition();
    void HandleSubLevelLoad();
    void HandleSubLevelUnload();
    void HandleWaveStart();
    void HandleDoorOpen();
    void HandleCustom();

    // === 유틸리티 함수들 ===
    
    bool CanActivate() const;
    bool IsPlayer(AActor* Actor) const;
    void LogTriggerInfo(const FString& Action) const;
};
