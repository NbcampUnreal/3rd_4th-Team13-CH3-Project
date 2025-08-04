#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MatrixGameplayAbility.generated.h"

class UMatrixAbilitySystemComponent;
class AEnemyCharacter;
class AEnemyAIController;

UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class MATRIX_API UMatrixGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UMatrixGameplayAbility();
    
    UFUNCTION(BlueprintCallable, Category = "Matrix|Ability")
    UMatrixAbilitySystemComponent* GetMatrixAbilitySystemComponentFromActorInfo() const;

    UFUNCTION(BlueprintCallable, Category = "Matrix|Ability")
    AEnemyCharacter* GetMatrixCharacterFromActorInfo() const;

    UFUNCTION(BlueprintCallable, Category = "Matrix|Ability")
    AEnemyAIController* GetMatrixAIControllerFromActorInfo() const;

protected:
};