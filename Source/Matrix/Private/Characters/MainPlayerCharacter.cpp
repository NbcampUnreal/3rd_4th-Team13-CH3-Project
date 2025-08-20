#include "Characters/MainPlayerCharacter.h"
#include "Characters/MainPlayerController.h"
#include "Weapons/WeaponSystem/WeaponBase.h"

#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "UI/Widget/WeaponHUDWidget.h"
#include "GameFramework/MatrixAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Kismet/GameplayStatics.h"


AMainPlayerCharacter::AMainPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set the team ID for the player
	TeamID = FGenericTeamId(5);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bDoCollisionTest = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	
	MoveSpeed = 500.f;
	bIsInput = false;
	bIsLook = false;
	bIsSlow = false;
	bCanHit = true;
	bCanShoot = true;
	CurrentWeapon = nullptr;

	// Set GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UMatrixAttributeSet>(TEXT("AttributeSet"));
}

FGenericTeamId AMainPlayerCharacter::GetGenericTeamId() const
{
	return TeamID;
}

UAbilitySystemComponent* AMainPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMainPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (AbilitySystemComponent && DeathAbilityClass)
	{
		FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DeathAbilityClass, 1, 0, this));
		
		if (!AbilityHandle.IsValid())
		{
	
		}
	}
}

void AMainPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SlowWorld(); //시간 제어 함수
}

void AMainPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (const AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController()))
		{
			//이동
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::Move
				);
			}
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AMainPlayerCharacter::EndInput //bIsInput = false
				);
			}
			//점프
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::StartJump
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AMainPlayerCharacter::StopJump
				);
			}
			//카메라 회전
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::Look
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Completed,
					this,
					&AMainPlayerCharacter::EndLook //bIsLook = false
				);
			}
			//공격
			if (PlayerController->ShootAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ShootAction,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::Shoot
				);
			}
			//상호작용(무기 줍기)
			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::Interact
				);
			}
			//퀵슬롯1
			if (PlayerController->QuickSlot1Action)
			{
				EnhancedInput->BindAction(
					PlayerController->QuickSlot1Action,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::QuickSlot1
				);
			}
			//퀵슬롯2
			if (PlayerController->QuickSlot2Action)
			{
				EnhancedInput->BindAction(
					PlayerController->QuickSlot2Action,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::QuickSlot2
				);
			}
			//퀵슬롯3
			if (PlayerController->QuickSlot3Action)
			{
				EnhancedInput->BindAction(
					PlayerController->QuickSlot3Action,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::QuickSlot3
				);
			}
			//시간조종(0.35)
			if (PlayerController->SlowTimeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SlowTimeAction,
					ETriggerEvent::Triggered,
					this,
					&AMainPlayerCharacter::StartSlow
				);
			}
			//시간조종 종료
			if (PlayerController->SlowTimeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SlowTimeAction,
					ETriggerEvent::Completed,
					this,
					&AMainPlayerCharacter::EndSlow
				);
			}
		}
	}
}

void AMainPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsInput = true; //입력을 받는 중으로 변경
	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AMainPlayerCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		bIsInput = true; //입력을 받는 중으로 변경
		Jump();
	}
}
void AMainPlayerCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
		EndInput(); //bIsInput = false
	}
}

void AMainPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	bIsLook = true; //회전 입력을 받는 중으로 변경(일반 입력이랑 다름)
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(-LookInput.Y);
}

//시간 제어 함수
void AMainPlayerCharacter::SlowWorld()
{
	float Dilation = 0.001f;

	if (bIsSlow)
		Dilation = 0.35f; 
	else if (bIsInput)
		Dilation = 1.f; 
	else if (bIsLook)
		Dilation = 0.3f; 
	else
		Dilation = 0.001f; 

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Dilation);
}

void AMainPlayerCharacter::Shoot(const FInputActionValue& Value)
{
	if (CurrentWeapon && bCanShoot)
	{
		CurrentWeapon->Shoot(); //총 발사
	}
}

//총기 장착 함수
void AMainPlayerCharacter::PickUpWeapon(AWeaponBase* NewWeapon)
{
	if (!NewWeapon) return;

	if (CurrentWeapon)
	{
		//CurrentWeapon->ResetWeaponOwner();
		CurrentWeapon->Destroy(); //기존 무기 Destroy
		CurrentWeapon = nullptr;
	}
	
	CurrentWeapon = NewWeapon; //새 무기 장착
	CurrentWeapon->SetWeaponOwner(this);
	CurrentWeapon->AttachToOwner(GetMesh());
	
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController()))
	{
		if (PlayerController->MainHUDWidgetInstance)
		{
			PlayerController->MainHUDWidgetInstance->UpdateWeaponIcon(CurrentWeapon->GetWeaponType());
			PlayerController->NotifyAmmoChanged(CurrentWeapon->GetCurrentBulletCount(), CurrentWeapon->GetMaxBulletCount());
		}
	}
}

void AMainPlayerCharacter::QuickSlot1(const FInputActionValue& Value)
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("[MainPlayerCharacter] Get Weapon Failed."))
		return;
	}
	
	if (Value.Get<bool>())
	{
		if (InventoryComp)
		{
			ABaseItem* Item = InventoryComp->GetItems(0);
			if (Item)
			{
				InventoryComp->RemoveItem(0);
			}
		}
	}
}

void AMainPlayerCharacter::QuickSlot2(const FInputActionValue& Value)
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("[MainPlayerCharacter] Get Weapon Failed."))
		return;
	}
	
	if (Value.Get<bool>())
	{
		if (InventoryComp)
		{
			ABaseItem* Item = InventoryComp->GetItems(1);
			if (Item)
			{
				InventoryComp->RemoveItem(1);
			}
		}
	}
}

void AMainPlayerCharacter::QuickSlot3(const FInputActionValue& Value)
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("[MainPlayerCharacter] Get Weapon Failed."))
		return;
	}
	
	if (Value.Get<bool>())
	{
		if (InventoryComp)
		{
			ABaseItem* Item = InventoryComp->GetItems(2);
			if (Item)
			{
				InventoryComp->RemoveItem(2);
			}
		}
	}
}

void AMainPlayerCharacter::Interact(const FInputActionValue& Value)
{
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CurrentWeapon);

	FVector Start = GetActorLocation();
	Start.Z += 60.f;

	FVector End = Start + CameraComp->GetForwardVector() * 200.f;
	FVector HalfSize = FVector(140.0f, 70.0f, 70.0f); // 박스 크기 지정
	FRotator Orientation = CameraComp->GetComponentRotation();
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResult,
		Start,
		End,
		Orientation.Quaternion(),
		ECC_OverlapAll_Deprecated,
		FCollisionShape::MakeBox(HalfSize),
		Params
	);

	if (bHit)
	{
		for (const FHitResult& HitActor : HitResult)
		{
			if (AWeaponBase* HitWeapon = Cast<AWeaponBase>(HitActor.GetActor()))
			{
				PickUpWeapon(HitWeapon);
				break;
			}
			else if (ABaseItem* HitItem = Cast<ABaseItem>(HitActor.GetActor()))
			{
				InventoryComp->AddItem(HitItem);
				break;
			}
		}
	}
	

	//DrawDebugBox(
	//	GetWorld(),
	//	(Start + End) * 0.5f,
	//	HalfSize,
	//	Orientation.Quaternion(),
	//	bHit ? FColor::Green : FColor::Red,
	//	false,
	//	2.0f
	//);
	//DrawDebugLine(
	//	GetWorld(),
	//	Start,
	//	End,
	//	FColor::Cyan,
	//	false,
	//	2.0f,
	//	0,
	//	2.0f
	//);
}

