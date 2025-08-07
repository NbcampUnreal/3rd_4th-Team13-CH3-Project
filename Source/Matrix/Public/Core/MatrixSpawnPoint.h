#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameplayTagAssetInterface.h"
#include "MatrixSpawnPoint.generated.h"

UCLASS()
class MATRIX_API AMatrixSpawnPoint : public ATargetPoint, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
	FGameplayTagContainer SpawnTags;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
};
