#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class MATRIX_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;
	
	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	bool bIsChasing = false;
	FTimerHandle ChaseTimer;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	void StartChasing(AActor* Target);
	void StopChasing();
	void UpdateChase();

	void StartAttacking();
	void StopAttacking();
	void PerformAttack();

private:
	void MoveToRandomLocation();

	FTimerHandle RandomMoveTimer;
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveRadius = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 800.0f;

	bool bIsAttacking = false;
};