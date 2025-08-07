#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "EnemyCharacter.generated.h"

class UMatrixAttributeSet;
class UAbilitySystemComponent;
class UGameplayAbility;
class AWeaponBase;

UCLASS()
class MATRIX_API AEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	UPROPERTY(EditAnywhere, Category = "AI")
	float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
	float RunSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxHealth;
	UPROPERTY(EditAnywhere, Category = "AI")
	float Health;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	bool bIsAttacking;

	void SetMovementSpeed(float NewSpeed);
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void FireProjectile();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	AWeaponBase* EquippedWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMatrixAttributeSet> AttributeSet;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> DeathAbilityClass;
	
	virtual void BeginPlay() override;
};