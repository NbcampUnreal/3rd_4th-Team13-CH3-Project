#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDamageComponent.generated.h"

class UGameplayEffect;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UWeaponDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponDamageComponent();

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* TargetActor, AActor* SourceActor, const FHitResult& HitResult);

	void SetBaseDamage(float Damage);
	void SetDamageEffect(TSubclassOf<UGameplayEffect> DamageEffect);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	void SendEventData(const FHitResult& HitResult, AActor* SourceActor);
};
