#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDropComponent.generated.h"

class UItemDropData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UItemDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemDropComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UItemDropData* DropData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DropChance;

	UFUNCTION(BlueprintCallable)
	void DropRandomItem(const FVector& SpawnLocation);
};
