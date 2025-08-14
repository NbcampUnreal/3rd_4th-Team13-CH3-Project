#include "Weapons/WeaponSystem/Component/WeaponDamageComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/MatrixAttributeSet.h"
#include "Weapons/WeaponSystem/BulletBase.h"

UWeaponDamageComponent::UWeaponDamageComponent()
	: BaseDamage(100.0f)
{
}

void UWeaponDamageComponent::ApplyDamage(AActor* TargetActor, AActor* SourceActor, const FHitResult& HitResult)
{
	if (!TargetActor) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor);

	if (SourceASC && TargetASC && DamageEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
			SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, BaseDamage);
			
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

			float CurrentHealth = TargetASC->GetNumericAttribute(UMatrixAttributeSet::GetHealthAttribute());
			if (CurrentHealth <= 0.0f)
			{
				SendEventData(HitResult, SourceActor);
			}
		}
	}
}

void UWeaponDamageComponent::SetBaseDamage(float Damage)
{
	BaseDamage = Damage;
}

void UWeaponDamageComponent::SetDamageEffect(TSubclassOf<UGameplayEffect> DamageEffect)
{
	DamageEffectClass = DamageEffect;	
}

void UWeaponDamageComponent::SendEventData(const FHitResult& HitResult, AActor* SourceActor)
{
	FVector NormalImpulse = HitResult.ImpactNormal * 3000.0f;

	FGameplayEventData EventData;
	EventData.Instigator = SourceActor;
	EventData.Target = HitResult.GetActor();
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));

	FTargetData_HitWithImpulse* TargetData = new FTargetData_HitWithImpulse();
	TargetData->ImpactPoint = HitResult.ImpactPoint;
	TargetData->Impulse = NormalImpulse;
	EventData.TargetData.Add(TargetData);
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), EventData.EventTag, EventData);
}
