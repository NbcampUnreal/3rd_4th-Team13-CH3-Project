#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UGA_Evade.generated.h"

UCLASS()
class MATRIX_API UUGA_Evade : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UUGA_Evade();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
