#include "Core/MatrixSpawnManager.h"
#include "Core/MatrixSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagAssetInterface.h"

AMatrixSpawnManager::AMatrixSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMatrixSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnPointsByTag.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMatrixSpawnPoint::StaticClass(), FoundActors);
	UE_LOG(LogTemp, Error, TEXT("SpawnManager: Found %d actors of class AMatrixSpawnPoint."), FoundActors.Num());
    
	for (AActor* Actor : FoundActors)
	{
		IGameplayTagAssetInterface* TaggedActor = Cast<IGameplayTagAssetInterface>(Actor);
		if (TaggedActor)
		{
			FGameplayTagContainer OwnedTags;
			TaggedActor->GetOwnedGameplayTags(OwnedTags);

			UE_LOG(LogTemp, Error, TEXT("SpawnManager: Actor '%s' has tags: [%s]"), *Actor->GetName(), *OwnedTags.ToString());
			
			for (const FGameplayTag& Tag : OwnedTags)
			{
				SpawnPointsByTag.FindOrAdd(Tag).Add(Actor);
			}
		}
	}
}

TArray<AActor*> AMatrixSpawnManager::GetSpawnPointsForTag(const FGameplayTag& InTag)
{
	const TArray<TObjectPtr<AActor>>* FoundArrayPtr = SpawnPointsByTag.Find(InTag);

	if (FoundArrayPtr)
	{
		TArray<AActor*> ResultArray;
		ResultArray.Reserve(FoundArrayPtr->Num());
		for (const TObjectPtr<AActor>& Ptr : *FoundArrayPtr)
		{
			if (Ptr)
			{
				ResultArray.Add(Ptr.Get());
			}
		}
		return ResultArray;
	}

	return TArray<AActor*>();
}

