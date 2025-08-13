#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "MagazineItem.generated.h"

UCLASS()
class MATRIX_API AMagazineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMagazineItem();
	
protected:
	virtual void UseItem() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magazine")
	float AddBulletAmount;
};
