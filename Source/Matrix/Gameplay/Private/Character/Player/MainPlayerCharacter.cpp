#include "Gameplay/MainPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EngineUtils.h"
#include "MainPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AMainPlayerCharacter::AMainPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

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
	float OtherActorsDilation = 0.001f; //캐릭터를 제외한 액터의 속도

	if (bIsInput) //입력을 받는중이면(최우선)
	{
		OtherActorsDilation = 1.f; 
	}
	else if (bIsLook) //카메라를 회전중이면
	{
		OtherActorsDilation = 0.3;
	}
	else //아무 입력도 받지 않으면
	{
		OtherActorsDilation = 0.001f; //0으로 하면 아예 게임이 멈춰버림.
	}

	for (TActorIterator<AActor> It(GetWorld()); It; ++It) //월드에 있는 모든 액터 탐색
	{
		AActor* Actor = *It;
		Actor->CustomTimeDilation = (Actor == this) ? 1.f : OtherActorsDilation; //액터가 자신(플레이어)인지에 따라서 속도 적용
	}
}

void AMainPlayerCharacter::Shoot(const FInputActionValue& Value)
{
	if (CurrentWeapon)
	{
		//CurrentWeapon->Shoot(); //총 발사
	}
}

void AMainPlayerCharacter::PickUpWeapon(AWeaponBase* NewWeapon)
{
	if (CurrentWeapon)
	{
		//CurrentWeapon->Destroy(); //기존 무기 제거
	}
	CurrentWeapon = NewWeapon; //새 무기 장착 
}

void AMainPlayerCharacter::Interact(const FInputActionValue& Value)
{
	//나중에 필요하면 구현
}