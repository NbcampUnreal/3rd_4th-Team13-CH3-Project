#include "Weapons/WeaponSystem/Component/WeaponDamageComponent.h"

UWeaponDamageComponent::UWeaponDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponDamageComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UWeaponDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

