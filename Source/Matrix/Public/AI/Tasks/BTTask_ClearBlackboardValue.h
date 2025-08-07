#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearBlackboardValue.generated.h"

UCLASS(Blueprintable)
class MATRIX_API UBTTask_ClearBlackboardValue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ClearBlackboardValue();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;
};
