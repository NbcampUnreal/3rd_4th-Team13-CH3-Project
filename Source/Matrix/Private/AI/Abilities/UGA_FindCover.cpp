#include "AI/Abilities/UGA_FindCover.h"
#include "AI/EnemyAIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UUGA_FindCover::UUGA_FindCover()
{
	// Set ability tags, cooldowns, costs here if needed
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.FindCover")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UUGA_FindCover::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UUGA_FindCover::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
