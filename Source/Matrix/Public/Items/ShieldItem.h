#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "ShieldItem.generated.h"

class UGameplayEffect;
class AMainPlayerCharacter;

UCLASS()
class MATRIX_API AShieldItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AShieldItem();
	
protected:
	virtual void UseItem() override;

	//지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldDuration;

	// 아이템 사용 시 발생하는 GameplayEffect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> GE_ShieldEffect;

	//지속시간 타이머
	FTimerHandle ShieldTimerHandle;

	UPROPERTY()
	AMainPlayerCharacter* OwnerCharacter;
};
