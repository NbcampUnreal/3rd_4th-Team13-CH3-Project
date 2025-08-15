#include "AI/Abilities/UGA_Attack.h"
#include "AI/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Animation/MatrixAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameplayTagsManager.h" // Added for Gameplay Tags
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h" // Added for UAbilityTask_WaitGameplayEvent
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Weapons/WeaponSystem/WeaponBase.h" // Include for AWeaponBase and EWeaponType

UUGA_Attack::UUGA_Attack()
{
	// Set ability tags, cooldowns, costs here if needed
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Melee")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UUGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(ActorInfo->AvatarActor.Get());
	if (!EnemyCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AWeaponBase* EquippedWeapon = EnemyCharacter->GetEquippedWeapon();
	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack: Enemy has no equipped weapon!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Get AI Controller and Blackboard
	AAIController* AIController = Cast<AAIController>(EnemyCharacter->GetController());
	UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;

	FVector TargetLocation = FVector::ZeroVector;
	AActor* TargetActor = nullptr;

	if (BlackboardComp)
	{
		TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
		if (TargetActor)
		{
			TargetLocation = TargetActor->GetActorLocation();
		}
		else
		{
			// Fallback to TargetLastKnownLocation if TargetActor is not set
			TargetLocation = BlackboardComp->GetValueAsVector(TEXT("TargetLastKnownLocation"));
		}
	}

	if (!TargetLocation.IsZero())
	{
		EquippedWeapon->SetTargetLocation(TargetLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack: Could not determine valid TargetLocation for weapon."));
	}

	EWeaponType CurrentWeaponType = EquippedWeapon->GetWeaponType();
	UAnimMontage* SelectedMontage = GetAttackMontageForWeapon(CurrentWeaponType);

	if (!SelectedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack: No montage found for weapon type %s!"), *UEnum::GetValueAsString(CurrentWeaponType));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SelectedMontage);
	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UUGA_Attack::OnMontageEnded);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UUGA_Attack::OnMontageEnded);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UUGA_Attack::OnMontageEnded);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UUGA_Attack::OnMontageEnded);
		PlayMontageTask->ReadyForActivation();

		// Listen for the FireBulletTag event from AnimNotify
		FGameplayTag FireBulletTag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.FireBullet"));
		UAbilityTask_WaitGameplayEvent* WaitFireBulletTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FireBulletTag, nullptr, true, true);
		if (WaitFireBulletTask)
		{
			WaitFireBulletTask->EventReceived.AddDynamic(this, &UUGA_Attack::OnFireBulletEvent);
			WaitFireBulletTask->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

UAnimMontage* UUGA_Attack::GetAttackMontageForWeapon(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		return PistolAttackMontage;
	case EWeaponType::Rifle:
		return RifleAttackMontage;
	case EWeaponType::Shotgun:
		return ShotgunAttackMontage;
	case EWeaponType::None:
	default:
		return nullptr;
	}
}

void UUGA_Attack::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UUGA_Attack::OnFireBulletEvent(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_Attack: OnFireBulletEvent called."));
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetAvatarActorFromActorInfo());
	if (EnemyCharacter)
	{
		EnemyCharacter->FireProjectile();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Attack: OnFireBulletEvent: EnemyCharacter is NULL. Cannot fire projectile."));
	}
}

// Gameplay Event 콜백 함수 구현
void UUGA_Attack::OnAttackHit(const FGameplayEventData& Payload)
{
	// 공격 히트 로직 (데미지 적용 등)
	// ...
}

void UUGA_Attack::OnAbilityEnd(const FGameplayEventData& Payload)
{
	// 어빌리티 종료 로직
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UUGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(ActorInfo->AvatarActor.Get());
	if (EnemyChar && EnemyChar->GetMesh() && EnemyChar->GetMesh()->GetAnimInstance())
	{
		UMatrixAnimInstance* MatrixAnimInstance = Cast<UMatrixAnimInstance>(EnemyChar->GetMesh()->GetAnimInstance());
		if (MatrixAnimInstance)
		{
			MatrixAnimInstance->bIsAttacking = false;
		}
	}
}

