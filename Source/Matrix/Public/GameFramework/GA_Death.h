#pragma once

#include "GameFramework/MatrixGameplayAbility.h"
#include "GA_Death.generated.h"

class AMainPlayerCharacter;
class AEnemyCharacter;

UCLASS(Blueprintable)
class MATRIX_API UGA_Death : public UMatrixGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Death();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Death|Chaos")
	TSubclassOf<AActor> ChaosDestructionClass;
	UPROPERTY(EditDefaultsOnly, Category = "Death|Chaos")
	float ChaosActorLifeSpan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death|Chaos")
	float ImpulseStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death|Chaos")
	float ImpulseRadius;
	
private:
	void HandlePlayerDeath(AMainPlayerCharacter* Player);
	void HandleAIDeath(AEnemyCharacter* Enemy);
	void SpawnChaosDestruction(AActor* OwnerActor, const FGameplayEventData* TriggerEventData);
};