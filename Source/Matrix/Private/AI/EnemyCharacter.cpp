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

	// 무기 클래스 매핑 초기화 (블루프린트에서 설정 가능)
	// 이 부분은 블루프린트에서 각 무기 유형에 해당하는 무기 클래스를 설정해야 합니다
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

void AEnemyCharacter::ChangeWeapon(EWeaponType NewWeaponType)
{
	UE_LOG(LogTemp, Warning, TEXT("ChangeWeapon called for %s with weapon type: %d"), *GetName(), (int32)NewWeaponType);
	UE_LOG(LogTemp, Warning, TEXT("WeaponClassMap has %d entries"), WeaponClassMap.Num());
	
	// WeaponClassMap 내용 출력
	for (const auto& Pair : WeaponClassMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("  WeaponType %d -> Class: %s"), 
			(int32)Pair.Key, 
			Pair.Value ? *Pair.Value->GetName() : TEXT("None"));
	}

	// 기존 무기 제거
	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Removing existing weapon: %s"), *EquippedWeapon->GetName());
		EquippedWeapon->DetachFromOwner();
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	// 새로운 무기 클래스 찾기
	TSubclassOf<AWeaponBase> NewWeaponClass = nullptr;
	
	if (WeaponClassMap.Contains(NewWeaponType))
	{
		NewWeaponClass = WeaponClassMap[NewWeaponType];
		UE_LOG(LogTemp, Warning, TEXT("Found weapon class in map: %s"), 
			NewWeaponClass ? *NewWeaponClass->GetName() : TEXT("None"));
	}
	else if (NewWeaponType == EWeaponType::None && DefaultWeaponClass)
	{
		// None인 경우 기본 무기 사용
		NewWeaponClass = DefaultWeaponClass;
		UE_LOG(LogTemp, Warning, TEXT("Using default weapon class: %s"), 
			NewWeaponClass ? *NewWeaponClass->GetName() : TEXT("None"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponType %d not found in WeaponClassMap and not using default"), (int32)NewWeaponType);
	}

	// 새 무기 스폰 및 장착
	if (NewWeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning new weapon: %s"), *NewWeaponClass->GetName());
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(NewWeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (EquippedWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon spawned successfully: %s"), *EquippedWeapon->GetName());
			EquippedWeapon->SetWeaponOwner(this);
			EquippedWeapon->AttachToOwner(GetMesh());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No weapon class available for type: %d"), (int32)NewWeaponType);
	}
}
