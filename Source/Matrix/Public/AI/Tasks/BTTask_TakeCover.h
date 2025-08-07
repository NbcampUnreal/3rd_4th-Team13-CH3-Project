#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbility.h"
#include "BTTask_TakeCover.generated.h"

UCLASS(Blueprintable)
class MATRIX_API UBTTask_TakeCover : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TakeCover();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> TakeCoverAbilityClass;
};
