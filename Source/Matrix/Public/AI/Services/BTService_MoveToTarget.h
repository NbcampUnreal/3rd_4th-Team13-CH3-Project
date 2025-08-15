#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_MoveToTarget.generated.h"

UCLASS()
class MATRIX_API UBTService_MoveToTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_MoveToTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MinDistance = 800.0f; // Minimum distance to maintain from target

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDistance = 1200.0f; // Maximum distance to maintain from target

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 50.0f; // How close to the target location is considered 'reached'

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementSpeed = 600.0f; // Speed to move towards target

	UPROPERTY(EditAnywhere, Category = "Avoidance")
	float AvoidanceRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Avoidance")
	float AvoidanceStrength = 0.5f;
};
