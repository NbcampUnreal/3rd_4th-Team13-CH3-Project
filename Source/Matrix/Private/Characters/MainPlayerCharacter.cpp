#include "Characters/MainPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Weapons/WeaponSystem/WeaponBase.h"
#include "Characters/MainPlayerController.h"
#include "UI/Widget/WeaponHUDWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


AMainPlayerCharacter::AMainPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bDoCollisionTest = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	MoveSpeed = 500.f;
	bIsInput = false;
	bIsLook = false;
	CurrentWeapon = nullptr;
}

void AMainPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	if (bIsInput)
		Dilation = 1.f; 
	else if (bIsLook)
		Dilation = 0.3f; 
	else
		Dilation = 0.001f; 

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Dilation);
}

void AMainPlayerCharacter::Shoot(const FInputActionValue& Value)
{
	if (CurrentWeapon)
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
		CurrentWeapon->ResetWeaponOwner();
		CurrentWeapon->Destroy(); //기존 무기 Destroy
		CurrentWeapon = nullptr;
	}
	
	CurrentWeapon = NewWeapon; //새 무기 장착
	// 무기를 캐릭터에 부착
	FAttachmentTransformRules AttachRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	true
	);	

	CurrentWeapon->AttachToComponent(GetMesh(), AttachRules, CurrentWeapon->GetAttachSocket());
	CurrentWeapon->SetWeaponOwner(this);


	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController()))
	{
		if (PlayerController->WeaponHUDWidgetInstance)
		{
			PlayerController->WeaponHUDWidgetInstance->UpdateWeaponIcon(CurrentWeapon->GetWeaponType());
		}

		PlayerController->NotifyAmmoChanged(CurrentWeapon->GetCurrentBulletCount(), CurrentWeapon->GetMaxBulletCount());
	}
}

void AMainPlayerCharacter::Interact(const FInputActionValue& Value)
{
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CurrentWeapon);

	FVector Start = GetActorLocation();
	FVector End = Start + CameraComp->GetForwardVector() * 350.0f;
	FVector HalfSize = FVector(10.0f, 100.0f, 100.0f); // 박스 크기 지정
	FRotator Orientation = CameraComp->GetComponentRotation();

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResult,
		Start,
		End,
		Orientation.Quaternion(),
		ECC_WorldStatic,
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
		}
	}

	DrawDebugBox(
	GetWorld(),
	(Start + End) * 0.5f,
	HalfSize,
	Orientation.Quaternion(),
	bHit ? FColor::Green : FColor::Red,
	false,
	2.0f
);
	DrawDebugLine(
	GetWorld(),
	Start,
	End,
	FColor::Cyan,
	false,
	2.0f,
	0,
	2.0f
);
}

