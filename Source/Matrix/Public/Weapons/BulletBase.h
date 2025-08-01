#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class UProjectileMovementComponent;

UCLASS()
class MATRIX_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletBase();

	void ActivateBullet(FVector Location, FRotator Rotation);
	void DeactivateBullet();
	FORCEINLINE bool IsActive() const { return bIsActive; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UProjectileMovementComponent* ProjectileMovementComp;

private:
	FTimerHandle DeactivateTimerHandle;
	bool bIsActive;
};
