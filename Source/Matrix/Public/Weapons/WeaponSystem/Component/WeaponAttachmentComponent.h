#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAttachmentComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UWeaponAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponAttachmentComponent();

	void AttachToOwner(USceneComponent* ParentComp);
	void DetachFromOwner();

protected:

};
