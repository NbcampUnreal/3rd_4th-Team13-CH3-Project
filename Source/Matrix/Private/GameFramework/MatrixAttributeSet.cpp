#include "GameFramework/MatrixAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

FGameplayAttribute UMatrixAttributeSet::GetHealthAttribute()
{
	static FProperty* Property = FindFieldChecked<FProperty>(UMatrixAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UMatrixAttributeSet, Health));
	return FGameplayAttribute(Property);
}

FGameplayAttribute UMatrixAttributeSet::GetMaxHealthAttribute()
{
	static FProperty* Property = FindFieldChecked<FProperty>(UMatrixAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UMatrixAttributeSet, MaxHealth));
	return FGameplayAttribute(Property);
}

UMatrixAttributeSet::UMatrixAttributeSet()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void UMatrixAttributeSet::SetHealth(float NewVal)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetHealthAttribute(), NewVal);
	}
}

float UMatrixAttributeSet::GetHealth() const
{
	return Health.GetCurrentValue();
}

void UMatrixAttributeSet::SetMaxHealth(float NewVal)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetMaxHealthAttribute(), NewVal);
	}
}

float UMatrixAttributeSet::GetMaxHealth() const
{
	return MaxHealth.GetCurrentValue();
}

void UMatrixAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UMatrixAttributeSet, Health, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMatrixAttributeSet, MaxHealth, COND_OwnerOnly, REPNOTIFY_Always);
}

void UMatrixAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMatrixAttributeSet, Health, OldHealth);
}

void UMatrixAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMatrixAttributeSet, MaxHealth, OldMaxHealth);
}

void UMatrixAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{

	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		// 현재 체력 값 로그 추가
		UE_LOG(LogTemp, Warning, TEXT("%s's Health changed to: %f"), *GetOwningActor()->GetName(), GetHealth()); // 추가
		
		//if (GetHealth() <= 0.0f)
		//{
		//	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		//	if (ASC)
		//	{
		//		FGameplayEventData Payload;
		//		Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));
		//		Payload.Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
		//		Payload.Target = GetOwningActor();
		//		Payload.ContextHandle = Data.EffectSpec.GetEffectContext();
		//		Payload.OptionalObject = Data.EffectSpec.GetEffectContext().GetSourceObject();
		//		Payload.EventMagnitude = Data.EvaluatedData.Magnitude;
		//		UE_LOG(LogTemp, Error, TEXT("Character %s is out of health. Sending GameplayEvent.Death!"), *GetOwningActor()->GetName());
		//		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		//	}
		//}
	}
}
