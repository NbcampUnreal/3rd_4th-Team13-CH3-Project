#include "Items/MagazineItem.h"
#include "Weapons/WeaponSystem/WeaponBase.h"
#include "Characters/MainPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AMagazineItem::AMagazineItem()
{
	ItemType = "Magazine";
	AddBulletAmount = 0.5f;
}

void AMagazineItem::UseItem()
{
	Super::UseItem();
	
	//캐릭터 캐스팅
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMainPlayerCharacter* OwnerCharacter = Cast<AMainPlayerCharacter>(PlayerPawn);
	//유효성 검사
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[MagazineItem] Character Casting Failed."));
		return;
	}
	
	//현재 무기 캐스팅
	AWeaponBase* CurrentWeapon = Cast<AWeaponBase>(OwnerCharacter->GetCurrentWeapon());
	//유효성 검사
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("[MagazineItem] Weapon Casting Failed."));
		return;
	}

	//AddBulletAmount만큼 최대 총알 갯수 비례 총알 추가
	CurrentWeapon->SetBulletCount(AddBulletAmount);
	UE_LOG(LogTemp, Warning, TEXT("[MagazineItem] Bullet Add. Amount: %f"), AddBulletAmount);
}