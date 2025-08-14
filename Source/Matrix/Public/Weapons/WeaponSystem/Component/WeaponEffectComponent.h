#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponEffectComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UWeaponEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponEffectComponent();

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void PlayEffect(FVector PlayLocation, FRotator PlayRotation);
	UFUNCTION(BlueprintCallable, Category = "Effect")
	void SetEffects(USoundBase* SFX, UParticleSystem* VFX);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UParticleSystem* FireEffect;
};
