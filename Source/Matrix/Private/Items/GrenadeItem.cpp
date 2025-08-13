#include "Items/GrenadeItem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Items/BombItem.h"
#include "Animation/AnimInstance.h"
#include "Characters/MainPlayerCharacter.h"

AGrenadeItem::AGrenadeItem()
{
	ItemType = "Grenade";
	bIsPickedUp = false;
}

void AGrenadeItem::UseItem()
{
	Super::UseItem();

	PlayThrowMontage();
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GrenadeItem/UseItem] PlayerPawn Get Failed."));
		return;
	}
	
	FVector SpawnLoc = PlayerPawn->GetActorLocation();
	FRotator SpawnRot = PlayerPawn->GetActorRotation();
	SpawnLoc.Z += 35.f;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerPawn;
	SpawnParams.Instigator = PlayerPawn;
	
	GetWorld()->SpawnActor<ABombItem>(BombItemClass, SpawnLoc, SpawnRot, SpawnParams);
}

void AGrenadeItem::PlayThrowMontage()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GrenadeItem/PlayMontage] PlayerPawn Get Failed."));
		return;
	}

	AMainPlayerCharacter* Character = Cast<AMainPlayerCharacter>(PlayerPawn);
	//던지는 동안 공격 불가
	Character->SetCanShoot(false);
	//애님 몽타주 재생
	if (Character && ThrowMontage)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(ThrowMontage))
		{
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindLambda([Character](UAnimMontage* Montage, bool bInterrupted)
			{
				Character->SetCanShoot(true);
			});
			AnimInstance->Montage_Play(ThrowMontage, 1.3);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ThrowMontage);
			UE_LOG(LogTemp, Warning, TEXT("[GrenadeItem/PlayMontage] Play Montage."));
		}
	}
}