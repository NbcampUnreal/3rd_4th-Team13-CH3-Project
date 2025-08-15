#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Weapons/WeaponSystem/WeaponBase.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h" // Added for UAbilityTask_WaitGameplayEvent
#include "UGA_Attack.generated.h"

UCLASS()
class MATRIX_API UUGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UUGA_Attack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> PistolAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> RifleAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ShotgunAttackMontage;

	UFUNCTION()
	void OnMontageEnded();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetAttackMontageForWeapon(EWeaponType WeaponType);

	UFUNCTION()
	void OnFireBulletEvent(FGameplayEventData Payload); // Added for handling bullet firing event

	UFUNCTION()
	void OnAbilityEnd(const FGameplayEventData& Payload);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	FDelegateHandle GenericEventHandle;
};
