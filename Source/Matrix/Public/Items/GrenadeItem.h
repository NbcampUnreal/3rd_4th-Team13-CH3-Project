#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "GrenadeItem.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class ABombItem;

UCLASS()
class MATRIX_API AGrenadeItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AGrenadeItem();
	
protected:
	virtual void UseItem() override;
	void PlayThrowMontage();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	TSubclassOf<ABombItem> BombItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Animation")
	UAnimMontage* ThrowMontage;
};
