#include "GameFramework/MatrixGameInstance.h"
#include "GameFramework/MatrixMaterialOverrideSystem.h"

#include "Kismet/GameplayStatics.h"

void UMatrixGameInstance::AddScore(int32 ScoreToAdd)
{
	PlayerScore += ScoreToAdd;
	UE_LOG(LogTemp, Warning, TEXT("Current Score : %d"), PlayerScore);
}

void UMatrixGameInstance::ResetGameData()
{
	PlayerScore = 0;
	CurrentLevel = 1;
	UE_LOG(LogTemp, Warning, TEXT("Game data has been reset."));
}

void UMatrixGameInstance::AdvanceToNextLevel()
{
	if (CurrentLevel < MaxLevel)
	{
		CurrentLevel++;
		UE_LOG(LogTemp, Warning, TEXT("Preparing to move to level %d"), CurrentLevel);
	}
	else
	{
		// 보스전 진입 시
		UE_LOG(LogTemp, Warning, TEXT("All levels cleared! Preparing for Boss Fight."));
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



