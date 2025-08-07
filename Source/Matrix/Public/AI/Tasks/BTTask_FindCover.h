#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbility.h"
#include "BTTask_FindCover.generated.h"

UCLASS(Blueprintable)
class MATRIX_API UBTTask_FindCover : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindCover();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CoverLocationKey;

	UPROPERTY(EditAnywhere, Category = "Cover")
	float CoverSearchRadius = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> FindCoverAbilityClass; // Added for GAS integration
};
