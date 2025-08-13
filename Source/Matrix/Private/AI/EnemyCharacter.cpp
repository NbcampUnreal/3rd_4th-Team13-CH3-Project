#include "AI/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "AI/EnemyAIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MatrixAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/WeaponSystem/WeaponBase.h"

AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = WalkSpeed;
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	Movement->AirControl = 0.2f;

	MaxHealth = 100.0f;
	Health = MaxHealth; 
	bIsAttacking = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UMatrixAttributeSet>(TEXT("AttributeSet"));
}

void AEnemyCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Error, TEXT("========== %s's BeginPlay HAS BEEN CALLED! =========="), *GetName());

	Super::BeginPlay();

	// --- 무기 장착 ---
	if (DefaultWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (EquippedWeapon)
		{
			EquippedWeapon->SetWeaponOwner(this);
			EquippedWeapon->AttachToOwner(GetMesh());
		}
	}

	// --- ASC 초기화 --- 
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		UE_LOG(LogTemp, Warning, TEXT("%s: ASC Initialized with ActorInfo."), *GetName());
	}

	// --- 어빌리티 부여 로직 ---
	if (AbilitySystemComponent && HasAuthority())
	{
		// Death Ability 부여
		if (DeathAbilityClass)
		{
			FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DeathAbilityClass, 1, 0, this));
			
			if (AbilityHandle.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s was successfully given the Death Ability! Handle: %s. DeathAbilityClass: %s"), *GetName(), *AbilityHandle.ToString(), *DeathAbilityClass->GetName()); 
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s: Failed to give Death Ability! Handle is invalid."), *GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s: DeathAbilityClass is NOT set! (GiveAbility failed)"), *GetName());
		}

		// Attack Ability 부여
		if (AttackAbilityClass)
		{
			FGameplayAbilitySpecHandle AttackAbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AttackAbilityClass, 1, 0, this));

			if (AttackAbilityHandle.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s was successfully given the Attack Ability! Handle: %s. AttackAbilityClass: %s"), *GetName(), *AttackAbilityHandle.ToString(), *AttackAbilityClass->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s: Failed to give Attack Ability! Handle is invalid."), *GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s: AttackAbilityClass is NOT set! (GiveAbility failed)"), *GetName());
		}
	}
	else
	{
		// AbilitySystemComponent 또는 HasAuthority가 유효하지 않을 경우
		if (!AbilitySystemComponent) UE_LOG(LogTemp, Error, TEXT("%s: AbilitySystemComponent is NULL! (GiveAbility failed)"), *GetName());
		if (!HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("%s: Is NOT Authority! (GiveAbility failed)"), *GetName());
	}
}


void AEnemyCharacter::SetMovementSpeed(float NewSpeed)
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = NewSpeed;
	}
}

UAbilitySystemComponent* AEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

AWeaponBase* AEnemyCharacter::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

void AEnemyCharacter::FireProjectile()
{
	UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter: FireProjectile() called."));
	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter: EquippedWeapon is VALID. Calling Shoot()."));
		EquippedWeapon->Shoot();
	}
	else
	{
		
	}
}