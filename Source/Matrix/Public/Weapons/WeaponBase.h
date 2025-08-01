#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class ABulletBase;
class UBulletPoolManager;

UCLASS()
class MATRIX_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

	UFUNCTION(BlueprintCallable)
	virtual void Shoot();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY()
	UBulletPoolManager* BulletPool;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TSubclassOf<ABulletBase> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float TriggerTime;

	virtual void BeginPlay() override;

private:
	bool IsShootAvailable;
	FTimerHandle ShootTriggerTimerHandle;

	void SetShootAvailable();
	void SetBulletPool();
};
