#include "Characters/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Core/MatrixCoreTypes.h"
#include "GameFramework/MatrixGameMode.h"
#include "GameFramework/MatrixGameState.h"
#include "GameFramework/MatrixGameInstance.h"
#include "GameFramework/MatrixLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MainPlayerCharacter.h"
#include "Engine/Engine.h"
#include "GameFramework/InventoryComponent.h"

AMainPlayerController::AMainPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	ShootAction(nullptr),
	InteractAction(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	OptionsMenuWidgetClass(nullptr),
	OptionsMenuWidgetInstance(nullptr),
	MainHUDWidgetClass(nullptr),
	MainHUDWidgetInstance(nullptr),
	PauseMenuWidgetClass(nullptr),
	PauseMenuWidgetInstance(nullptr),
	GameOverWidgetClass(nullptr),
	GameClearWidgetClass(nullptr),
	QuickSlot1Action(nullptr),
	QuickSlot2Action(nullptr),
	QuickSlot3Action(nullptr),
	SlowTimeAction(nullptr)
{
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input 설정
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	// 게임 상태 관리 초기화
	if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
	{
		OnGameStateChanged(GameInstance->GetPersistentGameState());
		GameInstance->OnGameStateChanged.AddDynamic(this, &AMainPlayerController::OnGameStateChanged);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this](){
			AMatrixGameState* MatrixGameState = GetWorld()->GetGameState<AMatrixGameState>();
			if (MatrixGameState)
			{
				MatrixGameState->OnGameStateChanged.AddDynamic(this, &AMainPlayerController::OnGameStateChanged);
				OnGameStateChanged(MatrixGameState->CurrentGameState);
			}
		}, 0.1f, false);
	}

	// 인벤토리 업데이트 이벤트 구독
	if (AMainPlayerCharacter* PC = Cast<AMainPlayerCharacter>(GetPawn()))
	{
		if (UInventoryComponent* InventoryComp = PC->GetInventoryComp())
		{
			InventoryComp->OnInventoryUpdated.AddDynamic(this, &AMainPlayerController::HandleInventoryUpdated);
		}
	}
}

void AMainPlayerController::OnGameStateChanged(EGameState NewState)
{
	// 현재 화면 위젯 제거
	if (CurrentScreenWidget)
	{
		CurrentScreenWidget->RemoveFromParent();
		CurrentScreenWidget = nullptr;
	}
    
	// 메인 HUD 숨기기
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
		UGameplayStatics::SetGamePaused(this, false);
		EnableInput(this);

		if (!MainHUDWidgetInstance && MainHUDWidgetClass) 
		{
			MainHUDWidgetInstance = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
			MainHUDWidgetInstance->AddToViewport();
		}

		if(MainHUDWidgetInstance) 
		{
			MainHUDWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());
		break;

	case EGameState::Paused:
		if (PauseMenuWidgetClass)
		{
			PauseMenuWidgetInstance = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
			PauseMenuWidgetInstance->AddToViewport();

			PauseMenuWidgetInstance->OnPauseMenuOptions.AddDynamic(this, &AMainPlayerController::OpenOptions);
			PauseMenuWidgetInstance->OnPauseMenuMainMenu.AddDynamic(this, &AMainPlayerController::GoToMainMenu);

			SetShowMouseCursor(true);
			PauseMenuWidgetInstance->SetIsFocusable(true);
			PauseMenuWidgetInstance->SetKeyboardFocus();

			FInputModeUIOnly IM;
			IM.SetWidgetToFocus(PauseMenuWidgetInstance->TakeWidget());
			IM.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(IM);

			CurrentScreenWidget = PauseMenuWidgetInstance;
		}
		break;

	case EGameState::GameOver:
		UGameplayStatics::SetGamePaused(this, true);
		DisableInput(this);

		if (GameOverWidgetClass)
		{
			CurrentScreenWidget = CreateWidget<UGameOverWidget>(this, GameOverWidgetClass);
			
			if (CurrentScreenWidget)
			{
				CurrentScreenWidget->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeUIOnly());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("MainPlayerController: Failed to create GameOver widget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MainPlayerController: GameOverWidgetClass is null"));
		}
		break;

	case EGameState::GameClear:
		UGameplayStatics::SetGamePaused(this, true);
		DisableInput(this);

		if (GameClearWidgetClass)
		{
			CurrentScreenWidget = CreateWidget<UGameClearWidget>(this, GameClearWidgetClass);

			if (CurrentScreenWidget)
			{
				CurrentScreenWidget->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeUIOnly());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("MainPlayerController: Failed to create GameClear widget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MainPlayerController: GameClearWidgetClass is null"));
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

void AMainPlayerController::NotifyKillCountChanged(int32 KillCount)
{
	if (MainHUDWidgetInstance)
	{
		MainHUDWidgetInstance->UpdateKillCount(KillCount);
	}
}

void AMainPlayerController::HandleInventoryUpdated()
{
	if (MainHUDWidgetInstance)
	{
		if (AMainPlayerCharacter* PC = Cast<AMainPlayerCharacter>(GetPawn()))
		{
			if (UInventoryComponent* InventoryComp = PC->GetInventoryComp())
			{
				MainHUDWidgetInstance->SlotHUDWidget->UpdateQuickSlots(InventoryComp->GetAllItems());
			}
		}
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

// === Game Flow Management ===

void AMainPlayerController::StartGame()
{
	if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
	{
		GameInstance->TransitionToLevel(TEXT("P_MainMap"), EGameState::Playing);
	}
	else
	{
		if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
		{
			LevelManager->StartGame();
		}
	}
}

void AMainPlayerController::OpenOptions(EGameState FromState)
{
	if (OptionsMenuWidgetInstance)
	{
		OptionsMenuWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		OptionsMenuWidgetInstance->SetIsFocusable(true);
		SetShowMouseCursor(true);

		FInputModeUIOnly IM;
		IM.SetWidgetToFocus(OptionsMenuWidgetInstance->TakeWidget());
		IM.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(IM);
		return;
	}

	PreviousState = FromState;

	if (OptionsMenuWidgetClass && !OptionsMenuWidgetInstance)
	{
		OptionsMenuWidgetInstance = CreateWidget<UOptionsMenuWidget>(this, OptionsMenuWidgetClass);
		if (OptionsMenuWidgetInstance)
		{
			OptionsMenuWidgetInstance->AddToViewport(100);
			OptionsMenuWidgetInstance->SetIsFocusable(true);
			
			OptionsMenuWidgetInstance->OnBackPressed.RemoveDynamic(this, &AMainPlayerController::CloseOptions);
			OptionsMenuWidgetInstance->OnBackPressed.AddDynamic(this, &AMainPlayerController::CloseOptions);

			OptionsMenuWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			SetShowMouseCursor(true);

			FInputModeUIOnly IM;
			IM.SetWidgetToFocus(OptionsMenuWidgetInstance->TakeWidget());
			IM.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(IM);
		}	
	}
}

void AMainPlayerController::CloseOptions()
{
	if (OptionsMenuWidgetInstance)
	{
		OptionsMenuWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}

	switch (PreviousState)
	{
		case EGameState::MainMenu:
		{
			if (MainMenuWidgetInstance)
			{
				SetShowMouseCursor(true);
				FInputModeUIOnly IM;
				IM.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
				IM.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(IM);
			}
			break;
		}
		case EGameState::Paused:
		{
			if (CurrentScreenWidget)
			{
				SetShowMouseCursor(true);
				FInputModeUIOnly IM;
				IM.SetWidgetToFocus(CurrentScreenWidget->TakeWidget());
				IM.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(IM);
			}
			break;
		}
		default:
		{
			SetShowMouseCursor(false);
			SetInputMode(FInputModeGameOnly());
			break;
		}
	}
}

void AMainPlayerController::EndGame(const FString& EndReason)
{
	if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
	{
		EGameState EndState = EGameState::GameOver;
		if (EndReason.Contains(TEXT("Clear")) || EndReason.Contains(TEXT("Win")))
		{
			EndState = EGameState::GameClear;
		}
		
		GameInstance->SetPersistentGameState(EndState);
	}
	else
	{
		if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
		{
			LevelManager->EndGame(EndReason);
		}
	}
}

void AMainPlayerController::GoToMainMenu()
{
	if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
	{
		GameInstance->TransitionToLevel(TEXT("L_MainMenu"), EGameState::MainMenu);
	}
	else
	{
		if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
		{
			LevelManager->GoToMainMenu();
		}
	}
}

void AMainPlayerController::RestartGame()
{
	if (UMatrixGameInstance* GameInstance = Cast<UMatrixGameInstance>(GetGameInstance()))
	{
		GameInstance->ResetGameData();
		GameInstance->TransitionToLevel(TEXT("P_MainMap"), EGameState::Playing);
	}
	else
	{
		if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
		{
			LevelManager->RestartGame();
		}
	}
}

void AMainPlayerController::OnMenuExit()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void AMainPlayerController::RequestLevelTransition(const FName& TargetLevel)
{
	if (UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>())
	{
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

// === Input Overrides ===

void AMainPlayerController::AddYawInput(float Val)
{
	Super::AddYawInput(Val * MouseSensitivity);
} 

void AMainPlayerController::AddPitchInput(float Val)
{
	Super::AddPitchInput(Val * MouseSensitivity);
}

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
// === Test Functions (Development Only) ===

void AMainPlayerController::TestGameOver()
{
	EndGame("Test Game Over");
}

void AMainPlayerController::TestGameClear()
{
	EndGame("Test Game Clear");
}

void AMainPlayerController::TestNextLevel()
{
	RequestLevelTransition("S_Floor1");
}
#endif