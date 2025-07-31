#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomLocation.generated.h"

UCLASS()
class MATRIX_API UBTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, Category = "Search", meta = (ClampMin = "100.0"))
	float SearchRadius = 1000.0f;
};
