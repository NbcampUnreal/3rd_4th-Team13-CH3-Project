#include "Items/ShieldItem.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"

#include "Characters/MainPlayerCharacter.h"

AShieldItem::AShieldItem()
{
	ItemType = "Shield";
	ShieldDuration = 3.f; //쉴드 지속시간
	OwnerCharacter = nullptr;
	GE_ShieldEffect = nullptr;
}

void AShieldItem::UseItem()
{
	Super::UseItem();

	if (!GE_ShieldEffect)
	{
		return;
	}
	
	//캐릭터 캐스팅
	if (!OwnerCharacter)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		OwnerCharacter = Cast<AMainPlayerCharacter>(PlayerPawn);
	}
		
	//유효성 검사
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShieldItem] Character Casting Failed."));
		return;
	}
	
	if (UAbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_ShieldEffect, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		}
	}
}