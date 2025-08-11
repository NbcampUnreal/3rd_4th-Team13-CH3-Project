#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEC_BulletDamage.generated.h"

UCLASS()
class MATRIX_API UGEC_BulletDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGEC_BulletDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
