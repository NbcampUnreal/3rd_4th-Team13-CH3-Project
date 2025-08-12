#include "Animation/AnimNotify_FireProjectile.h"
#include "AI/EnemyCharacter.h"

void UAnimNotify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(MeshComp->GetOwner());
		if (EnemyCharacter)
		{
			EnemyCharacter->FireProjectile();
		}
	}
}