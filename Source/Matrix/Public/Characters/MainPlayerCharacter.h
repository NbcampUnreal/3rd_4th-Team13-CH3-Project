#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h" // Required for Team ID
#include "MainPlayerCharacter.generated.h"

class UInventoryComponent;
class UGameplayAbility;
class UMatrixAttributeSet;
class AWeaponBase;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class MATRIX_API AMainPlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AMainPlayerCharacter();

	//~ IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface

	//~ IGenericTeamAgentInterface
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComp;

	void SetCanShoot(bool NewCanShoot) {bCanShoot = NewCanShoot;};
	void SetCanHit(bool NewCanHit) {bCanHit = NewCanHit;};
	AWeaponBase* GetCurrentWeapon() {return CurrentWeapon;};
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeaponBase* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UMatrixAttributeSet> AttributeSet;		// 체력 AttributeSet
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> DeathAbilityClass;	// 사망 Ability

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bCanHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bCanShoot;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void Shoot(const FInputActionValue& Value);
	UFUNCTION()
	void Interact(const FInputActionValue& Value);
	UFUNCTION()
	void QuickSlot1(const FInputActionValue& Value);
	UFUNCTION()
	void QuickSlot2(const FInputActionValue& Value);
	UFUNCTION()
	void QuickSlot3(const FInputActionValue& Value);
	
	//void OnDeath();
	void PickUpWeapon(AWeaponBase* NewWeapon);
	void SlowWorld(); //시간 제어 함수
	void EndInput() {bIsInput = false;};
	void EndLook() {bIsLook = false;};
	void StartSlow() {bIsSlow = true;};
	void EndSlow() {bIsSlow = false;};
	
private:
	float MoveSpeed;
	bool bIsInput;
	bool bIsLook;
	bool bIsSlow;
};
