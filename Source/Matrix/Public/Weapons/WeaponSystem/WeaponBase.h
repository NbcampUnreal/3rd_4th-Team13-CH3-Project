#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UWeaponFireComponent;
class UWeaponEffectComponent;
class UWeaponDamageComponent;
class UWeaponAttachmentComponent;
class UGameplayEffect;
class UArrowComponent;
class USphereComponent;
class ABulletBase;
class ABulletPoolManager;

//무기 리스트 열거형
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None		UMETA(DisplayName="None"),
	Rifle		UMETA(DisplayName="Rifle"),
	Pistol  	UMETA(DisplayName="Pistol"),
	Shotgun 	UMETA(DisplayName="Shotgun")
};

UCLASS()
class MATRIX_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	
	void SetWeaponOwner(AActor* NewOwner);
	void ResetWeaponOwner();
	void AttachToOwner(USceneComponent* CharacterMesh);
	void DetachFromOwner();
	void ApplyBulletDamage(AActor* TargetActor, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void Shoot();
	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(const FVector& NewTargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Bullet")
	FORCEINLINE int32 GetMaxBulletCount() const { return MaxBulletCount; };
	UFUNCTION(BlueprintCallable, Category = "Bullet")
	FORCEINLINE int32 GetCurrentBulletCount() const { return CurrentBulletCount; };
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE UStaticMeshComponent* GetMeshComp() const { return MeshComp; }

	FVector GetFireDirection() const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UArrowComponent* MuzzlePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UWeaponAttachmentComponent* AttachmentComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UWeaponDamageComponent* DamageComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UWeaponEffectComponent* EffectComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	EWeaponType WeaponType;
	UPROPERTY()
	APawn* OwnerPawn;
	UPROPERTY()
	AController* OwnerPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float TriggerTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 MaxBulletCount;
	int32 CurrentBulletCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire")
	bool bIsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TSubclassOf<ABulletBase> BulletClass;
	UPROPERTY()
	ABulletPoolManager* BulletPoolManager;
	UPROPERTY()
	FVector TargetLocation;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool FireBullet();
	
private:
	FTimerHandle ShootTriggerTimer;
	
	void SetShootAvailable();
	void SetBulletPool();
};
