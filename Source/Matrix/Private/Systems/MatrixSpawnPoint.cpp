#include "Systems/MatrixSpawnPoint.h"

void AMatrixSpawnPoint::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = SpawnTags;
}
