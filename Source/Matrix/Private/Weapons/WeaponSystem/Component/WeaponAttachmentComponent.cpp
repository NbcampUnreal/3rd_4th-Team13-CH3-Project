#include "Weapons/WeaponSystem/Component/WeaponAttachmentComponent.h"

UWeaponAttachmentComponent::UWeaponAttachmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponAttachmentComponent::AttachToOwner(USceneComponent* ParentComp)
{

}

void UWeaponAttachmentComponent::DetachFromOwner()
{
}
