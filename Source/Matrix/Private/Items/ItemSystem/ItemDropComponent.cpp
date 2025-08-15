#include "Items/ItemSystem/ItemDropComponent.h"

#include "Items/ItemSystem/ItemDropData.h"

UItemDropComponent::UItemDropComponent()
	: DropData(nullptr)
	, DropChance(0.5f)
{
}

void UItemDropComponent::DropRandomItem(const FVector& SpawnLocation)
{
	if (!DropData || DropData->DropItems.Num() == 0)
	{
		return;
	}

	if (FMath::FRand() < DropChance)
	{
		float SumDropChance = 0.0f;
		for (const FItemDropEntry& DropEntry : DropData->DropItems)
		{
			if (DropEntry.ItemClass)
			{
				SumDropChance += DropEntry.DropChance;
			}
		}

		float RandomItemValue = FMath::FRandRange(0.0f, SumDropChance);
		float AccumulatedChance = 0.0f;

		for (const FItemDropEntry& DropEntry : DropData->DropItems)
		{
			if (!DropEntry.ItemClass)
			{
				continue;
			}

			AccumulatedChance += DropEntry.DropChance;
			if (RandomItemValue < AccumulatedChance)
			{
				GetWorld()->SpawnActor<AActor>(DropEntry.ItemClass, SpawnLocation, FRotator::ZeroRotator);
				UE_LOG(LogTemp, Warning, TEXT("DropRandomItem: Spawned Item: %s"), *DropEntry.ItemClass->GetName());
				
				break;	
			}
		}
	}
}
