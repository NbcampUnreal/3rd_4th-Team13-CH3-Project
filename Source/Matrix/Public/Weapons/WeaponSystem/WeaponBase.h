#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

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
	void AttachToOwner(USceneComponent* ParentComp);
	void DetachFromOwner();
	void ApplyBulletDamage(AActor* TargetActor, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void Shoot();
	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(const FVector& NewTargetLocation);
	
	UFUNCTION(BlueprintCallable, Category = "Bullet")
	FORCEINLINE int32 GetMaxBulletCount() const { return MaxBulletCount; }
	UFUNCTION(BlueprintCallable, Category = "Bullet")
	FORCEINLINE int32 GetCurrentBulletCount() const { return CurrentBulletCount; }
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE FName GetAttachSocket() const { return AttachSocket; }
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE UStaticMeshComponent* GetMeshComp() const { return MeshComp; }
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FVector GetFireDirection() const;		// 총알 발사 방향 계산 함수
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TSubclassOf<ABulletBase> BulletClass;
	UPROPERTY()
	ABulletPoolManager* BulletPoolManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float TriggerTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int32 MaxBulletCount;
	int32 CurrentBulletCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	EWeaponType WeaponType; //무기 Enum 멤버 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsFiring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> BulletDamageEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName AttachSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FRotator MeshInitialRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FVector MeshInitialScale;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UParticleSystem* FireEffect;
	
	UPROPERTY()
	APawn* OwnerPawn;
	UPROPERTY()
	AController* OwnerPC;
	UPROPERTY()
	FVector TargetLocation;
	
	virtual void BeginPlay() override;
	virtual bool FireBullet();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FTimerHandle ShootTriggerTimer;

	void SetShootAvailable();
	void SetBulletPool();
	void SendEventData(const FHitResult& HitResult);
};
