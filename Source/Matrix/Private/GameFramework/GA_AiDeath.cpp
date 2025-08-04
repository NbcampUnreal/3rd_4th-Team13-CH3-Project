#include "GameFramework/GA_AiDeath.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"

UGA_AiDeath::UGA_AiDeath()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

bool UGA_AiDeath::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (!bCanActivate)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_AiDeath: CanActivateAbility returned FALSE for %s. Reason: Super::CanActivateAbility failed."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
		return false;
	}

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Death.Dead"))))
		{
			UE_LOG(LogTemp, Error, TEXT("GA_AiDeath: CanActivateAbility returned FALSE for %s. Reason: Already Dead Tag present."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
			return false;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GA_AiDeath: CanActivateAbility returned TRUE for %s. Attempting to activate."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
	return true;
}

void UGA_AiDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Error, TEXT("ActivateAbility"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("GA_AiDeath has been ACTIVATED for %s!"), *GetAvatarActorFromActorInfo()->GetName());

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Death.Dead")));
		UE_LOG(LogTemp, Warning, TEXT("%s: Added Status.Death.Dead tag."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->DisableMovement();
	}

	if (DeathMontage)
	{
		const float MontageLength = Character->PlayAnimMontage(DeathMontage);
		if (MontageLength > 0.f)
		{
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &UGA_AiDeath::EndAbility, Handle, ActorInfo, ActivationInfo, true, false);
			
			FTimerHandle DeathTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, Delegate, MontageLength, false);
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_AiDeath::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Death.Dead")));
		UE_LOG(LogTemp, Warning, TEXT("%s: Removed Status.Death.Dead tag."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
	}

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		AvatarActor->SetLifeSpan(2.0f);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}