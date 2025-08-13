#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class MATRIX_API ABombItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABombItem();

	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; };
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(VisibleAnywhere, Category="Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float InitialSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion")
	float ExplosionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion")
	float ExplosionTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;
	
	void Explode();

	FTimerHandle TimerHandle_Explosion;
};
