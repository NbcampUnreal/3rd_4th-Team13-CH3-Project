#include "AI/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "AI/EnemyAIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MatrixAttributeSet.h"
#include "Items/ItemSystem/ItemDropComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/WeaponSystem/WeaponBase.h"

AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Set the team ID for this AI character
	TeamID = FGenericTeamId(10);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = WalkSpeed;
	Movement->bOrientRotationToMovement = false; // Keep this as false if you want controller to dictate facing
	bUseControllerRotationYaw = false; // Disable direct controller yaw rotation
	bUseControllerRotationPitch = false; // Ensure pitch is also not directly controlled
	bUseControllerRotationRoll = false; // Ensure roll is also not directly controlled

	// Allow CharacterMovementComponent to smoothly rotate towards controller's desired rotation
	Movement->bUseControllerDesiredRotation = true;
	Movement->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // Set a turning speed (e.g., 360 degrees per second)
	Movement->AirControl = 0.2f;

	MaxHealth = 100.0f;
	Health = MaxHealth; 
	bIsAttacking = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UMatrixAttributeSet>(TEXT("AttributeSet"));

	ItemDropComp = CreateDefaultSubobject<UItemDropComponent>(TEXT("ItemDropComp"));
}

FGenericTeamId AEnemyCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void AEnemyCharacter::BeginPlay()
{
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
	}

	// --- 어빌리티 부여 로직 ---
	if (AbilitySystemComponent && HasAuthority())
	{
		// Death Ability 부여
		if (DeathAbilityClass)
		{
			FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DeathAbilityClass, 1, 0, this));
		}

		// Attack Ability 부여
		if (AttackAbilityClass)
		{
			FGameplayAbilitySpecHandle AttackAbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AttackAbilityClass, 1, 0, this));
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

UItemDropComponent* AEnemyCharacter::GetItemDropComp() const
{
	return ItemDropComp;
}

void AEnemyCharacter::FireProjectile()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Shoot();
	}
}
