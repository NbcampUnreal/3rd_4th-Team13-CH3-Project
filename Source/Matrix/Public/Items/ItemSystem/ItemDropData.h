#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDropData.generated.h"

USTRUCT(BlueprintType)
struct FItemDropEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DropChance;
};

UCLASS(BlueprintType)
class MATRIX_API UItemDropData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FItemDropEntry> DropItems;
};
