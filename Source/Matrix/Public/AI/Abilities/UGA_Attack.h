#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UGA_Attack.generated.h"

UCLASS()
class MATRIX_API UUGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UUGA_Attack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Gameplay Event 콜백 함수
	UFUNCTION()
	void OnAttackHit(const FGameplayEventData& Payload);

	UFUNCTION()
	void OnAbilityEnd(const FGameplayEventData& Payload);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
