#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponEffectComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UWeaponEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponEffectComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
