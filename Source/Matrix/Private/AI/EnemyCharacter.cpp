#include "AI/EnemyCharacter.h"
#include "AbilitySystemComponent.h" // Added back
#include "AI/EnemyAIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MatrixAttributeSet.h" // Added back
#include "Kismet/GameplayStatics.h"
#include "Projectiles/Projectile.h"

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

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UMatrixAttributeSet>(TEXT("AttributeSet"));
}

void AEnemyCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Error, TEXT("========== %s's BeginPlay HAS BEEN CALLED! =========="), *GetName());

	Super::BeginPlay();

	// --- ASC 초기화 --- 
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		UE_LOG(LogTemp, Warning, TEXT("%s: ASC Initialized with ActorInfo."), *GetName());
	}

	// --- 어빌리티 부여 로직 ---
	if (AbilitySystemComponent && HasAuthority() && DeathAbilityClass)
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
		// if문이 실패했을 경우를 확인하기 위한 로그
		if (!AbilitySystemComponent) UE_LOG(LogTemp, Error, TEXT("%s: AbilitySystemComponent is NULL! (GiveAbility failed)"), *GetName());
		if (!HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("%s: Is NOT Authority! (GiveAbility failed)"), *GetName());
		if (!DeathAbilityClass) UE_LOG(LogTemp, Error, TEXT("%s: DeathAbilityClass is NOT set! (GiveAbility failed)"), *GetName());
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

void AEnemyCharacter::FireProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass is not set for %s"), *GetName());
		return;
	}

	FVector HandLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
	FRotator Rotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, HandLocation, Rotation, SpawnParams);
	if (Projectile)
	{
		if (FireMontage)
		{
			PlayAnimMontage(FireMontage);
		}
		
		FVector LaunchDirection = Rotation.Vector();
		Projectile->FireInDirection(LaunchDirection);
	}
}
