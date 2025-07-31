#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayerCharacter.generated.h"

class AWeaponBase;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class MATRIX_API AMainPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	
protected:
	AWeaponBase* CurrentWeapon;
	
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
	
	//void OnDeath();
	void PickUpWeapon(AWeaponBase* NewWeapon);
	void SlowWorld(); //시간 제어 함수
	void EndInput() {bIsInput = false;};
	void EndLook() {bIsLook = false;};
	
private:
	float MoveSpeed;
	bool bIsInput;
	bool bIsLook;
};
