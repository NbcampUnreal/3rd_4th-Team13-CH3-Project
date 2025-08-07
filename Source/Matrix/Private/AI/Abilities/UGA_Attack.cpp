#include "AI/Abilities/UGA_Attack.h"
#include "AI/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Animation/MatrixAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameplayTagsManager.h" // Added for Gameplay Tags

UUGA_Attack::UUGA_Attack()
{
	// Set ability tags, cooldowns, costs here if needed
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Melee")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UUGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(ActorInfo->AvatarActor.Get());
	if (!EnemyChar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // Fail if character is invalid
		return;
	}

	// Example: Play an attack animation (replace with your actual montage)
	// if (EnemyChar->AttackMontage)
	// {
	//     EnemyChar->PlayAnimMontage(EnemyChar->AttackMontage);
	//     // You would typically end the ability on an animation notify
	//     // For now, we'll end it immediately for instant abilities.
	// }
	
	// Example: Triggering animation in ABP by setting a boolean variable
	if (EnemyChar->GetMesh() && EnemyChar->GetMesh()->GetAnimInstance())
	{
		// Assuming you have a boolean variable named "bIsAttacking" in your Animation Blueprint
		UMatrixAnimInstance* MatrixAnimInstance = Cast<UMatrixAnimInstance>(EnemyChar->GetMesh()->GetAnimInstance());
		if (MatrixAnimInstance)
		{
			MatrixAnimInstance->bIsAttacking = true;
		}
	}
	EnemyChar->FireProjectile(); // Keep this if you still want to fire projectile 

	// Example: Applying damage directly to the player (if target is known)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
	    // Calculate damage amount
	    float DamageAmount = 10.0f; 
	    // Apply damage (e.g., using UGameplayStatics::ApplyDamage or a GameplayEffect)
	    UGameplayStatics::ApplyDamage(PlayerPawn, DamageAmount, EnemyChar->GetController(), EnemyChar, UDamageType::StaticClass());
	}
	
	// Gameplay Event 리스너 등록
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		// 공격 히트 이벤트 리스너
		ASC->GenericGameplayEventCallbacks.FindOrAdd(FGameplayTag::RequestGameplayTag(FName("Event.Attack.Hit"))).AddLambda([this](const FGameplayEventData* Payload)
		{
			if (Payload)
			{
				OnAttackHit(*Payload);
			}
		});
		// 어빌리티 종료 이벤트 리스너
		ASC->GenericGameplayEventCallbacks.FindOrAdd(FGameplayTag::RequestGameplayTag(FName("Event.Ability.End"))).AddLambda([this](const FGameplayEventData* Payload)
		{
			if (Payload)
			{
				OnAbilityEnd(*Payload);
			}
		});
	}

	// For now, we'll just assume the location is found and set in Blackboard by BTTask_FindCover
	// and the movement will be handled by BTTask_MoveTo.
	// This ability primarily serves as a trigger and a place for more complex cover-finding logic.

	// If you are using EQS, you would end the ability in the OnQueryFinished callback.
	// For instant abilities, end it immediately.
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

// Gameplay Event 콜백 함수 구현
void UUGA_Attack::OnAttackHit(const FGameplayEventData& Payload)
{
	// 공격 히트 로직 (데미지 적용 등)
	UE_LOG(LogTemp, Warning, TEXT("Attack Hit Event Received!"));
	// ...
}

void UUGA_Attack::OnAbilityEnd(const FGameplayEventData& Payload)
{
	// 어빌리티 종료 로직
	UE_LOG(LogTemp, Warning, TEXT("Ability End Event Received!"));
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UUGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Gameplay Event 리스너 해제 (중요!)
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		ASC->GenericGameplayEventCallbacks.Remove(FGameplayTag::RequestGameplayTag(FName("Event.Attack.Hit")));
		ASC->GenericGameplayEventCallbacks.Remove(FGameplayTag::RequestGameplayTag(FName("Event.Ability.End")));
	}

	// Example: Resetting the boolean variable in ABP when ability ends
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
