#include "Weapons/WeaponSystem/Component/WeaponEffectComponent.h"

UWeaponEffectComponent::UWeaponEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UWeaponEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

