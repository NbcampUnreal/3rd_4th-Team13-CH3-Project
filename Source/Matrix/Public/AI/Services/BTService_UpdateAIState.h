
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateAIState.generated.h"

UCLASS()
class MATRIX_API UBTService_UpdateAIState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateAIState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceToTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsInAttackRangeKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 800.0f;
};
