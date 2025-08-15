#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_InvalidateAndSearch.generated.h"

UCLASS()
class MATRIX_API UBTTask_InvalidateAndSearch : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_InvalidateAndSearch();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastKnownPlayerLocationKey;
};
