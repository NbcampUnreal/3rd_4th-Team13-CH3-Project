#include "Characters/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Core/MatrixCoreTypes.h"
#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "GameFramework/MatrixLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MainPlayerCharacter.h"
#include "Engine/Engine.h"

AMainPlayerController::AMainPlayerController()
	: InputMappingContext(nullptr), 
	MoveAction(nullptr), 
	JumpAction(nullptr),
	LookAction(nullptr), 
	ShootAction(nullptr), 
	InteractAction(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	MainHUDWidgetClass(nullptr),
	MainHUDWidgetInstance(nullptr),
	QuickSlot1Action((nullptr)),
	QuickSlot2Action((nullptr)),
	QuickSlot3Action((nullptr)),
	SlowTimeAction((nullptr))
{
	
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this](){
		AMatrixGameState* MatrixGameState = GetWorld()->GetGameState<AMatrixGameState>();
		if (MatrixGameState)
		{
			MatrixGameState->OnGameStateChanged.AddDynamic(this, &AMainPlayerController::OnGameStateChanged);

			OnGameStateChanged(MatrixGameState->CurrentGameState);
		}
	}, 0.1f, false);
	
	/*	OnGameStateChanged 함수가 역할을 대신합니다! 혹시몰라 주석처리만 해놨어요!
	if (MainHUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}

	if (WeaponHUDWidgetClass)
	{
		WeaponHUDWidgetInstance = CreateWidget<UWeaponHUDWidget>(this, WeaponHUDWidgetClass);
		if (WeaponHUDWidgetInstance)
		{
			WeaponHUDWidgetInstance->AddToViewport();
		}
	}
	*/
}


void AMainPlayerController::OnGameStateChanged(EGameState NewState)
{

	UE_LOG(LogTemp, Warning, TEXT("[PC] State Changed -> %d"), (int32)NewState);
	// 화면을 덮는 위젯이 있다면 일단 제거
	if (CurrentScreenWidget)
	{
		CurrentScreenWidget->RemoveFromParent();
		CurrentScreenWidget = nullptr;
	}
    
	// 메인 HUD도 상태에 따라 껐다 켰다 할 수 있어요!
	if(MainHUDWidgetInstance)
	{
		MainHUDWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}

	switch (NewState)
	{
	case EGameState::MainMenu:
		if (!MainMenuWidgetInstance && MainMenuWidgetClass)
		{
			MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
			if (MainMenuWidgetInstance)
			{
				MainMenuWidgetInstance->AddToViewport();

				MainMenuWidgetInstance->OnStartRequested.AddDynamic(this, &AMainPlayerController::StartGame);
				MainMenuWidgetInstance->OnOptionsRequested.AddDynamic(this, &AMainPlayerController::OpenOptions);
				MainMenuWidgetInstance->OnExitRequested.AddDynamic(this, &AMainPlayerController::OnMenuExit);
			}
		}

		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}

		SetShowMouseCursor(true);
		SetInputMode(FInputModeUIOnly());
		break;

	case EGameState::Playing:
		if (!MainHUDWidgetInstance && MainHUDWidgetClass) 
		{
			MainHUDWidgetInstance = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
			MainHUDWidgetInstance->AddToViewport();
		}

		if(MainHUDWidgetInstance) MainHUDWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());
		break;

	case EGameState::Paused:
		if (PauseMenuClass)
		{
			CurrentScreenWidget = CreateWidget<UUserWidget>(this, PauseMenuClass);
			CurrentScreenWidget->AddToViewport();
			SetShowMouseCursor(true);
			SetInputMode(FInputModeGameAndUI()); // 게임은 멈춰있지만 UI와 상호작용
		}
		break;

	case EGameState::GameOver:
		if (GameOverWidgetClass)
		{
			CurrentScreenWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
			CurrentScreenWidget->AddToViewport();
			SetShowMouseCursor(true);
			SetInputMode(FInputModeUIOnly());
		}
		break;

	case EGameState::GameClear:
		if (GameClearWidgetClass)
		{
			CurrentScreenWidget = CreateWidget<UUserWidget>(this, GameClearWidgetClass);
			CurrentScreenWidget->AddToViewport();
			SetShowMouseCursor(true);
			SetInputMode(FInputModeUIOnly());
		}
		break;
        
	default:
		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());
		break;
	}
}


void AMainPlayerController::NotifyAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (MainHUDWidgetInstance)
	{
		MainHUDWidgetInstance->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pause", IE_Pressed, this, &AMainPlayerController::HandlePauseMenu);
}

void AMainPlayerController::HandlePauseMenu()
{
	AMatrixGameMode* GameMode = GetWorld()->GetAuthGameMode<AMatrixGameMode>();
	if (GameMode)
	{
		GameMode->RequestTogglePause();
	}
}

// === 게임 흐름 관리 함수들 ===

void AMainPlayerController::StartGame()
{
	if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
		LevelManager->StartGame();
	}
}

void AMainPlayerController::OpenOptions()
{
	
}

void AMainPlayerController::EndGame(const FString& EndReason)
{
	if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
		LevelManager->EndGame(EndReason);
	}
}

void AMainPlayerController::GoToMainMenu()
{
	if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
		LevelManager->GoToMainMenu();
	}
}

void AMainPlayerController::RestartGame()
{
	if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
		LevelManager->RestartGame();
	}
}

void AMainPlayerController::OnMenuExit()
{
	UKismetSystemLibrary::QuitGame(
		this,
		this,
		EQuitPreference::Quit,
		false
	);
}

void AMainPlayerController::RequestLevelTransition(const FName& TargetLevel)
{
	if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
		// 기본적으로 일반 전환으로 처리
		LevelManager->RequestLevelTransition(TargetLevel, ELevelTransitionType::NextLevel);
	}
}

bool AMainPlayerController::IsInGame() const
{
	if (const UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
		return LevelManager->IsInGame();
	}
	return false;
}

// === 테스트용 함수들 ===

void AMainPlayerController::TestGameOver()
{
	UE_LOG(LogTemp, Log, TEXT("Testing Game Over"));
	EndGame("Test Game Over");
}

void AMainPlayerController::TestGameClear()
{
	UE_LOG(LogTemp, Log, TEXT("Testing Game Clear"));
	EndGame("Test Game Clear");
}

void AMainPlayerController::TestNextLevel()
{
	UE_LOG(LogTemp, Log, TEXT("Testing Next Level"));
	RequestLevelTransition("S_Floor1");
}

/* 게임 모드에서 Pause를 관리할 수 있도록 구조를 좀 바꿨습니다. 혹시 몰라 주석처리해놓습니다.
void AMainPlayerController::HandlePauseMenu()
{
	if (!bIsPaused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		bIsPaused = true;

		if (PauseMenuClass)
		{
			PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
			if (PauseMenuInstance)
			{
				PauseMenuInstance->AddToViewport();

				PauseMenuInstance->SetIsFocusable(true);
				PauseMenuInstance->SetKeyboardFocus();

				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputMode);
				bShowMouseCursor = true;
			}
		}
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		bIsPaused = false;

		if (PauseMenuInstance)
		{
			PauseMenuInstance->RemoveFromParent();
			PauseMenuInstance = nullptr;
		}

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}
*/