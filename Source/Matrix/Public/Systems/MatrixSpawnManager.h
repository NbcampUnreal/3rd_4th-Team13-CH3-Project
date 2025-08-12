#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "MatrixSpawnManager.generated.h"

UCLASS()
class MATRIX_API AMatrixSpawnManager : public AActor
{
    GENERATED_BODY()

public:
    AMatrixSpawnManager();

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    TArray<AActor*> GetSpawnPointsForTag(const FGameplayTag& InTag);

protected:
    virtual void BeginPlay() override;

private:
    TMap<FGameplayTag, TArray<TObjectPtr<AActor>>> SpawnPointsByTag;
};
