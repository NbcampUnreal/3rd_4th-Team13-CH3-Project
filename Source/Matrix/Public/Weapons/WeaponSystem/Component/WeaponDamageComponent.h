#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDamageComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UWeaponDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponDamageComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
