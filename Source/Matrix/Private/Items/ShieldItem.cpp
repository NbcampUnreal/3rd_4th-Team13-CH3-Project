#include "Items/ShieldItem.h"
#include "Characters/MainPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AShieldItem::AShieldItem()
{
	ItemType = "Shield";
	ShieldDuration = 3.f; //쉴드 지속시간
	OwnerCharacter = nullptr;
}

void AShieldItem::UseItem()
{
	Super::UseItem();
	
	//캐릭터 캐스팅
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	OwnerCharacter = Cast<AMainPlayerCharacter>(PlayerPawn);
	//유효성 검사
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShieldItem] Character Casting Failed."));
		return;
	}

	//캐릭터의 bCanHit을 false로 설정
	OwnerCharacter->SetCanHit(false);

	//지속시간 타이머 설정
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		ShieldTimerHandle,
		this,
		&AShieldItem::EndShield,
		ShieldDuration,
		false
		);

	UE_LOG(LogTemp, Warning, TEXT("[ShieldItem] Character Shield activated: %fs."), ShieldDuration);
}

void AShieldItem::EndShield()
{
	
	if (OwnerCharacter)
	{
		OwnerCharacter->SetCanHit(true);
		UE_LOG(LogTemp, Warning, TEXT("[ShieldItem] Remove Effect."));
	}
}