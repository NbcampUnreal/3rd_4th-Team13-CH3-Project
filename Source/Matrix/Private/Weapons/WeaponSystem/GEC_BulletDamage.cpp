#include "Weapons/WeaponSystem/GEC_BulletDamage.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/MatrixAttributeSet.h"

UGEC_BulletDamage::UGEC_BulletDamage()
{
}

void UGEC_BulletDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                               FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (TargetASC && TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Status.BlockOneHit")))
	{
		TargetASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Status.BlockOneHit")));

		return;
	}
	
	static FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	float Damage = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(DamageTag, false);

	if (Damage > 0.f)
	{
		if (AActor* TargetActor = TargetASC->GetAvatarActor())
		{
		
		}
		
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UMatrixAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
	}
}
