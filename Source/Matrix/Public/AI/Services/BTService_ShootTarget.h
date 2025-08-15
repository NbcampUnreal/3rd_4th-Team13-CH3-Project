#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ShootTarget.generated.h"

UCLASS()
class MATRIX_API UBTService_ShootTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ShootTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasLineOfSightKey;
};
