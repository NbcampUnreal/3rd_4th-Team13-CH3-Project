#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage; // Added for damage sense

UCLASS()
class MATRIX_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	// This function will be called from the player or game mode to progress the AI's logic
	void ExecuteTurn(float PlayerMovementDistance);

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Damage* DamageConfig; // Added for damage sense
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;
	
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	void PerformAttack();

	UFUNCTION()
	void OnPlayerDied(); // New function to handle player death

protected:
	// Called in Blueprint when the player is seen or lost
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnPlayerSeen(AActor* PlayerActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnPlayerLost();

	// Called in Blueprint when a projectile is detected
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnProjectileDetected();

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveRadius = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 800.0f;
};