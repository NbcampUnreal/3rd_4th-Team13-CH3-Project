#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbility.h"
#include "BTTask_Evade.generated.h"

UCLASS(Blueprintable)
class MATRIX_API UBTTask_Evade : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Evade();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Evade")
	float EvadeDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> EvadeAbilityClass; // Added for GAS integration
};
