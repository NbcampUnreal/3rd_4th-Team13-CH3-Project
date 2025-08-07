#include "Core/MatrixSpawnPoint.h"

void AMatrixSpawnPoint::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(SpawnTags);
}