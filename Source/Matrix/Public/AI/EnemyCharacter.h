#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "EnemyCharacter.generated.h"

class UItemDropComponent;
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

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AWeaponBase* GetEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	UItemDropComponent* GetItemDropComp() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	AWeaponBase* EquippedWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UItemDropComponent> ItemDropComp;

	UPROPERTY()
	TObjectPtr<UMatrixAttributeSet> AttributeSet;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> DeathAbilityClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> AttackAbilityClass; // <--- ADDED THIS LINE
	
	virtual void BeginPlay() override;
};