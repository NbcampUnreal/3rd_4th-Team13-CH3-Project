#include "AI/Abilities/UGA_Evade.h"
#include "AI/EnemyAIController.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/MatrixAnimInstance.h"

UUGA_Evade::UUGA_Evade()
{
	// Set ability tags, cooldowns, costs here if needed
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Evade")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UUGA_Evade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Character->GetController());
	if (!EnemyAIController)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComp();
	if (!BlackboardComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FVector EvadeLocation = BlackboardComp->GetValueAsVector(FName("TargetLocation"));

	// --- Implement your evade logic here ---
	// This is a basic example. You'll likely want to:
	// 1. Trigger animation in ABP.
	// 2. Use CharacterMovementComponent to perform a dash/jump.
	// 3. Apply temporary invulnerability or speed boost.

	// Example: Triggering animation in ABP by setting a boolean variable
	if (Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		// Assuming you have a boolean variable named "bIsEvading" in your Animation Blueprint
		// Character->GetMesh()->GetAnimInstance()->SetCustomBoolProperty("bIsEvading", true); // Pseudocode
	}

	// Example: Perform a simple dash/jump
	// Character->LaunchCharacter(FVector(0, 0, 500), false, true); // Jump straight up
	// Character->GetCharacterMovement()->AddImpulse(Character->GetActorForwardVector() * 1000.0f, true); // Dash forward

	// Example: Move to the calculated evade location
	EnemyAIController->MoveToLocation(EvadeLocation);

	// If the ability is instant or movement is handled by the ability, end it.
	// For movement abilities, you might wait for movement completion or animation end.
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UUGA_Evade::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Example: Resetting the boolean variable in ABP when ability ends
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		UMatrixAnimInstance* MatrixAnimInstance = Cast<UMatrixAnimInstance>(Character->GetMesh()->GetAnimInstance());
		if (MatrixAnimInstance)
		{
			MatrixAnimInstance->bIsEvading = false;
		}
	}
}
