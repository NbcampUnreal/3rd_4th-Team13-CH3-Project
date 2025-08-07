// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "EnemyCharacter.generated.h"

class UMatrixAttributeSet;
class UAbilitySystemComponent;
class UGameplayAbility;

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

	void SetMovementSpeed(float NewSpeed);
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void FireProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	class AWeaponBase* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMatrixAttributeSet> AttributeSet;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> DeathAbilityClass;
	
	virtual void BeginPlay() override;
};
