#include "GameFramework/MatrixGameInstance.h"
#include "GameFramework/MatrixMaterialOverrideSystem.h"
#include "GameFramework/MatrixGameState.h"
#include "Characters/MainPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void UMatrixGameInstance::AddScore(int32 ScoreToAdd)
{
	PlayerScore += ScoreToAdd;
}

void UMatrixGameInstance::ResetGameData()
{
	PlayerScore = 0;
	CurrentLevel = 1;
}

void UMatrixGameInstance::AdvanceToNextLevel()
{
	if (CurrentLevel < MaxLevel)
	{
		CurrentLevel++;
	}
	else
	{
		// 보스전 진입 시
	}
}

void UMatrixGameInstance::InitializeMaterialOverrideSystem()
{
	if (!MaterialOverrideSystem)
	{
		MaterialOverrideSystem = NewObject<UMatrixMaterialOverrideSystem>(this);
	}
}

void UMatrixGameInstance::ApplyWhiteMaterialToWorld()
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->ApplyWhiteMaterialToAllMeshes();
	}
}

void UMatrixGameInstance::ApplyWhiteMaterialToAllMeshes()
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		// 먼저 무효한 참조들을 정리
		MaterialOverrideSystem->CleanupInvalidReferences();
		// 그 다음 머티리얼 적용
		MaterialOverrideSystem->ApplyWhiteMaterialToAllMeshes();
	}
}

void UMatrixGameInstance::ApplyWhiteMaterialWithDelay(float DelaySeconds)
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->ApplyWhiteMaterialWithDelay(DelaySeconds);
	}
}

void UMatrixGameInstance::StartRepeatingMaterialApplication(float IntervalSeconds, int32 MaxAttempts)
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->StartRepeatingMaterialApplication(IntervalSeconds, MaxAttempts);
	}
}

void UMatrixGameInstance::StopRepeatingMaterialApplication()
{
	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->StopRepeatingMaterialApplication();
	}
}

void UMatrixGameInstance::RestoreOriginalMaterials()
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->RestoreOriginalMaterials();
	}
}

void UMatrixGameInstance::CleanupInvalidReferences()
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->CleanupInvalidReferences();
	}
}

void UMatrixGameInstance::ApplyMaterialToActorsByNamePattern(const FString& NamePattern)
{
	if (!MaterialOverrideSystem)
	{
		InitializeMaterialOverrideSystem();
	}

	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->ApplyMaterialToActorsByNamePattern(NamePattern);
	}
}

void UMatrixGameInstance::DebugPrintAllActors() const
{
	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->DebugPrintAllActors();
	}
}

void UMatrixGameInstance::DebugPrintActorMaterials(const FString& ActorName) const
{
	if (MaterialOverrideSystem)
	{
		MaterialOverrideSystem->DebugPrintActorMaterials(ActorName);
	}
}

void UMatrixGameInstance::SetPersistentGameState(EGameState NewState)
{
	if (PersistentGameState != NewState)
	{
		PersistentGameState = NewState;
		
		// 이벤트 브로드캐스트
		OnGameStateChanged.Broadcast(NewState);
		
		// 현재 World의 GameState도 동기화
		if (UWorld* World = GetWorld())
		{
			if (AMatrixGameState* MatrixGameState = World->GetGameState<AMatrixGameState>())
			{
				MatrixGameState->SetGameState(NewState);
			}
		}
	}
}

void UMatrixGameInstance::TransitionToLevel(const FString& LevelName, EGameState TargetState)
{
	// 게임 시작 시 게임 데이터 초기화
	if (TargetState == EGameState::Playing)
	{
		ResetGameData();
	}
	
	// 레벨 전환 전에 상태 저장
	SetPersistentGameState(TargetState);
	
	// 레벨 전환
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
	
	// 레벨 전환 완료 후 상태 복원을 위한 타이머 설정
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		OnLevelTransitionComplete();
	}, 0.2f, false);
}

void UMatrixGameInstance::RestoreGameStateAfterTransition()
{
	// 현재 World의 GameState 동기화
	if (UWorld* World = GetWorld())
	{
		if (AMatrixGameState* MatrixGameState = World->GetGameState<AMatrixGameState>())
		{
			MatrixGameState->SetGameState(PersistentGameState);
		}
	}
	
	// PlayerController들에게 상태 복원 알림
	TArray<APlayerController*> PlayerControllers;
	GetWorld()->GetPlayerControllerIterator();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			if (AMainPlayerController* MPC = Cast<AMainPlayerController>(PC))
			{
				// PlayerController에게 현재 상태 알림
				MPC->OnGameStateChanged(PersistentGameState);
			}
		}
	}
}

void UMatrixGameInstance::OnLevelTransitionComplete()
{
	RestoreGameStateAfterTransition();
}



