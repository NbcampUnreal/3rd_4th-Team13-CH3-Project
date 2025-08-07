#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "UGA_FindCover.generated.h"

class UEnvQuery;

UCLASS()
class MATRIX_API UUGA_FindCover : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UUGA_FindCover();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditAnywhere, Category = "EQS")
	UEnvQuery* FindCoverQuery; // Your EQS query asset

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CoverLocationKey; // Key to store the found cover location
};
