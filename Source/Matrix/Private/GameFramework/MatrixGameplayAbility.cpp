#include "GameFramework/MatrixGameplayAbility.h"
#include "GameFramework/MatrixAbilitySystemComponent.h"
#include "AI/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "AbilitySystemComponent.h"

UMatrixGameplayAbility::UMatrixGameplayAbility()
{
}

UMatrixAbilitySystemComponent* UMatrixGameplayAbility::GetMatrixAbilitySystemComponentFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<UMatrixAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AEnemyCharacter* UMatrixGameplayAbility::GetMatrixCharacterFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AEnemyCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

AEnemyAIController* UMatrixGameplayAbility::GetMatrixAIControllerFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AEnemyAIController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}