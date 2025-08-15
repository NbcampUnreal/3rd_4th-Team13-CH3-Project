#include "GameFramework/GA_Death.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

#include "AI/EnemyCharacter.h"
#include "GameFramework/MatrixGameMode.h"
#include "Characters/MainPlayerCharacter.h"
#include "Items/ItemSystem/ItemDropComponent.h"
#include "Weapons/WeaponSystem/BulletBase.h"
#include "Weapons/WeaponSystem/WeaponBase.h"

UGA_Death::UGA_Death()
	: ChaosActorLifeSpan(3.0f)
	, ImpulseStrength(1000.0f)
	, ImpulseRadius(100.0f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

bool UGA_Death::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (!bCanActivate)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_AiDeath: CanActivateAbility returned FALSE for %s. Reason: Super::CanActivateAbility failed."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
		return false;
	}

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Death.Dead"))))
		{
			UE_LOG(LogTemp, Error, TEXT("GA_AiDeath: CanActivateAbility returned FALSE for %s. Reason: Already Dead Tag present."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
			return false;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GA_AiDeath: CanActivateAbility returned TRUE for %s. Attempting to activate."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
	return true;
}

void UGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		UE_LOG(LogTemp, Error, TEXT("OwnerActor is NULL - GA_AiDeath!"));
		return;
	}

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Death.Dead")));
		UE_LOG(LogTemp, Warning, TEXT("%s: Added Status.Death.Dead tag."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
	}

	if (AMainPlayerCharacter* Player = Cast<AMainPlayerCharacter>(OwnerActor))
	{
		HandlePlayerDeath(Player);
	}
	else if (AEnemyCharacter* AI = Cast<AEnemyCharacter>(OwnerActor))
	{
		HandleAIDeath(AI);
	}
	
	// ChaosSystem 파괴 연출
	if (ChaosDestructionClass)
	{
		SpawnChaosDestruction(OwnerActor, TriggerEventData);
	}

	// 메시 & 충돌 & 이동 비활성화
	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		Character->GetMesh()->SetVisibility(false);
		Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Character->GetCharacterMovement())
		{
			Character->GetCharacterMovement()->DisableMovement();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	//{
	//	ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Death.Dead")));
	//	UE_LOG(LogTemp, Warning, TEXT("%s: Removed Status.Death.Dead tag."), *GetNameSafe(ActorInfo->AvatarActor.Get()));
	//}

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		AvatarActor->SetLifeSpan(2.0f);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Death::HandlePlayerDeath(AMainPlayerCharacter* Player)
{
	if (!Player) return;

	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		PC->DisableInput(PC);
	}

	if (AWeaponBase* EquippedWeapon = Player->GetCurrentWeapon())
	{
		EquippedWeapon->ResetWeaponOwner();
		EquippedWeapon->DetachFromOwner();
	}

	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld()))
	{
		if (AMatrixGameMode* MatrixGameMode = Cast<AMatrixGameMode>(GM))
		{
			MatrixGameMode->PlayerDied();
			UE_LOG(LogTemp, Log, TEXT("Player Death: Notified GameMode of player death."));
		}
	}
}

void UGA_Death::HandleAIDeath(AEnemyCharacter* Enemy)
{
	if (!Enemy) return;

	if (AWeaponBase* EquippedWeapon = Enemy->GetEquippedWeapon())
	{
		EquippedWeapon->ResetWeaponOwner();
		EquippedWeapon->DetachFromOwner();
	}
	
	if (UItemDropComponent* ItemDropComponent = Enemy->GetItemDropComp())
	{
		ItemDropComponent->DropRandomItem(Enemy->GetActorLocation());
	}
	
	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld()))
	{
		if (AMatrixGameMode* MatrixGameMode = Cast<AMatrixGameMode>(GM))
		{
			MatrixGameMode->EnemyKilled();
			UE_LOG(LogTemp, Log, TEXT("GA_Death: Notified GameMode of enemy death."));
		}
	}
}

void UGA_Death::SpawnChaosDestruction(AActor* OwnerActor, const FGameplayEventData* TriggerEventData)
{
	if (!OwnerActor || !ChaosDestructionClass) return;

	FVector SpawnLoc = OwnerActor->GetActorLocation();
	SpawnLoc.Z -= 90.0f;
	FRotator SpawnRot = OwnerActor->GetActorRotation();
	SpawnRot.Yaw -= 90.0f;

	AGeometryCollectionActor* ChaosActor = OwnerActor->GetWorld()->SpawnActor<AGeometryCollectionActor>(ChaosDestructionClass, SpawnLoc, SpawnRot);
	
	if (ChaosActor)
	{
		ChaosActor->SetLifeSpan(ChaosActorLifeSpan);

		if (UGeometryCollectionComponent* GeometryCollection = ChaosActor->GetGeometryCollectionComponent())
		{
			GeometryCollection->SetSimulatePhysics(true);
			
			TArray<float> DamageThresholds;
			DamageThresholds.Add(0.0f);
			GeometryCollection->SetDamageThreshold(DamageThresholds);

			const float FinalImpulseStrength = ImpulseStrength;
			const float FinalImpulseRadius = ImpulseRadius;
			FVector HitLocation = OwnerActor->GetActorLocation();

			if (TriggerEventData && TriggerEventData->TargetData.Num() > 0)
			{
				if (const FTargetData_HitWithImpulse* HitData = static_cast<const FTargetData_HitWithImpulse*>(TriggerEventData->TargetData.Get(0)))
				{
					HitLocation = HitData->ImpactPoint;

					GeometryCollection->AddImpulseAtLocation(HitData->Impulse, HitLocation);
					UE_LOG(LogTemp, Warning, TEXT("GA_AiDeath: Added Impulse at %s."), *HitLocation.ToString());
				}
			}

			GetWorld()->GetTimerManager().SetTimerForNextTick([GeometryCollection, HitLocation, FinalImpulseStrength, FinalImpulseRadius]()
			{
				GeometryCollection->AddRadialImpulse(HitLocation, FinalImpulseRadius, FinalImpulseStrength, RIF_Linear, true);
			});
		}
	}
	
}
