#include "AI/Abilities/UGA_Evade.h"
#include "AI/EnemyAIController.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UUGA_Evade::UUGA_Evade()
{
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

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector CharacterLocation = Character->GetActorLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector DirectionFromPlayer = (CharacterLocation - PlayerLocation).GetSafeNormal();

	// Calculate a perpendicular direction for evasion
	FVector EvadeDirection = FVector::CrossProduct(DirectionFromPlayer, FVector::UpVector).GetSafeNormal();
	if (FMath::RandBool())
	{
		EvadeDirection *= -1.0f;
	}

	float EvadeDistance = 500.0f; // Distance to evade
	FVector EvadeLocation = CharacterLocation + EvadeDirection * EvadeDistance;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FNavLocation RandomLocation;
		if (NavSys->GetRandomReachablePointInRadius(EvadeLocation, 200.0f, RandomLocation))
		{
			EnemyAIController->MoveToLocation(RandomLocation.Location);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UUGA_Evade::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
