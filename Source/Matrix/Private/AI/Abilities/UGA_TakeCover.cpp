#include "AI/Abilities/UGA_TakeCover.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/MatrixAnimInstance.h"

UUGA_TakeCover::UUGA_TakeCover()
{
	// Set ability tags, cooldowns, costs here if needed
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.FindCover")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UUGA_TakeCover::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// --- Implement your take cover logic here ---
	// This could involve:
	// 1. Trigger animation in ABP.
	// 2. Changing character stance (e.g., Character->Crouch(); Character->GetCharacterMovement()->SetCrouchedHalfHeight(45.0f);)
	// 3. Applying temporary defensive buffs (e.g., damage reduction via GameplayEffect)
	// 4. Adjusting character collision

	// Example: Triggering animation in ABP by setting a boolean variable
	if (Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		// Assuming you have a boolean variable named "bIsTakingCover" in your Animation Blueprint
		// Character->GetMesh()->GetAnimInstance()->SetCustomBoolProperty("bIsTakingCover", true); // Pseudocode
	}

	// Example: Basic crouch
	if (Character->GetCharacterMovement()->IsCrouching())
	{
		Character->UnCrouch();
	}
	else
	{
		Character->Crouch();
	}

	// If the ability is instant, end it immediately.
	// For animation-driven abilities, you'd end it via an animation notify or a timer.
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UUGA_TakeCover::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Example: Resetting the boolean variable in ABP when ability ends
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		UMatrixAnimInstance* MatrixAnimInstance = Cast<UMatrixAnimInstance>(Character->GetMesh()->GetAnimInstance());
		if (MatrixAnimInstance)
		{
			MatrixAnimInstance->bIsTakingCover = false;
		}
	}
}
