#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "ShieldItem.generated.h"

class AMainPlayerCharacter;

UCLASS()
class MATRIX_API AShieldItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AShieldItem();
	
protected:
	virtual void UseItem() override;

	//쉴드 종료
	void EndShield();

	//지속시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldDuration;

	//지속시간 타이머
	FTimerHandle ShieldTimerHandle;
	
	AMainPlayerCharacter* OwnerCharacter;
};
