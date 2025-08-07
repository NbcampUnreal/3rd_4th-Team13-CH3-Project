#include "Animation/MatrixAnimInstance.h"
#include "AbilitySystemComponent.h" // GAS 관련 헤더
#include "GameFramework/Character.h"
#include "AI/EnemyCharacter.h" // 필요하다면 캐릭터 클래스 헤더

void UMatrixAnimInstance::AnimNotify_AttackHit()
{
	AEnemyCharacter* OwningCharacter = Cast<AEnemyCharacter>(GetOwningActor());
	if (OwningCharacter)
	{
		UAbilitySystemComponent* ASC = OwningCharacter->GetAbilitySystemComponent();
		if (ASC)
		{
			// Gameplay Event 발생 (가장 권장되는 방법)
			FGameplayEventData EventData;
			EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.Hit")); // 정의된 Gameplay Tag
			ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
		}
	}
}

void UMatrixAnimInstance::AnimNotify_AbilityEnd()
{
	AEnemyCharacter* OwningCharacter = Cast<AEnemyCharacter>(GetOwningActor());
	if (OwningCharacter)
	{
		UAbilitySystemComponent* ASC = OwningCharacter->GetAbilitySystemComponent();
		if (ASC)
		{
			FGameplayEventData EventData;
			EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.End"));
			ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
		}
	}
}

// No specific implementation needed in the .cpp for a simple AnimInstance base class
// unless you have custom C++ logic for it.
