#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateFromSharedLocation.generated.h"

UCLASS()
class MATRIX_API UBTService_UpdateFromSharedLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateFromSharedLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastKnownPlayerLocationKey;
};
