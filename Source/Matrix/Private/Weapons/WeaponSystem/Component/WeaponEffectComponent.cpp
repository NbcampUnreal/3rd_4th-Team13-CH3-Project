#include "Weapons/WeaponSystem/Component/WeaponEffectComponent.h"

#include "Kismet/GameplayStatics.h"

UWeaponEffectComponent::UWeaponEffectComponent()
	: FireSound(nullptr)
	, FireEffect(nullptr)
{
}

void UWeaponEffectComponent::PlayEffect(FVector PlayLocation, FRotator PlayRotation, FVector ParticleScale)
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, PlayLocation);
	}

	if (FireEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, FireEffect, PlayLocation, PlayRotation, ParticleScale);
	}
}

void UWeaponEffectComponent::SetEffects(USoundBase* SFX, UParticleSystem* VFX)
{
	FireSound = SFX;
	FireEffect = VFX;
}


