#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "MatrixAbilityCost.generated.h"

class UMatrixGameplayAbility;

UCLASS(MinimalAPI, DefaultToInstanced, EditInlineNew, Abstract)
class UMatrixAbilityCost : public UObject
{
	GENERATED_BODY()

public:
	UMatrixAbilityCost() {}

	virtual bool CheckCost(const UMatrixGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}

	virtual void ApplyCost(const UMatrixGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
	{
	}

	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	bool bOnlyApplyCostOnHit = false;
};
