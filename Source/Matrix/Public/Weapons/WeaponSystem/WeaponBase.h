#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class ABulletBase;
class ABulletPoolManager;

UCLASS()
class MATRIX_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

	UFUNCTION(BlueprintCallable)
	virtual void Shoot();
	
	UFUNCTION(BlueprintCallable, Category = "Bullet")
	FORCEINLINE int32 GetMaxBulletCount() const { return MaxBulletCount; }
	UFUNCTION(BlueprintCallable, Category = "Bullet")
	FORCEINLINE int32 GetCurrentBulletCount() const { return CurrentBulletCount; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TSubclassOf<ABulletBase> BulletClass;
	UPROPERTY()
	ABulletPoolManager* BulletPoolManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float TriggerTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int32 MaxBulletCount;
	int32 CurrentBulletCount;

	virtual void BeginPlay() override;

private:
	bool IsShootAvailable;
	FTimerHandle ShootTriggerTimerHandle;

	void SetShootAvailable();
	void SetBulletPool();
};
