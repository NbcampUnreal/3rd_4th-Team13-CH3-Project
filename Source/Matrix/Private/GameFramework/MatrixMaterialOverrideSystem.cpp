#include "GameFramework/MatrixMaterialOverrideSystem.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UMatrixMaterialOverrideSystem::UMatrixMaterialOverrideSystem()
{
	ExcludeTags = { TEXT("Player"), TEXT("Enemy") };
}

void UMatrixMaterialOverrideSystem::ApplyWhiteMaterialToWorld()
{
	ApplyWhiteMaterialToAllMeshes();
}

void UMatrixMaterialOverrideSystem::ApplyWhiteMaterialToAllMeshes()
{
	// WhiteMaterial 로직 삭제됨 - BasicShapeMaterial 사용

	UWorld* World = GetWorld();
	if (!World || !IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	// 원래 머티리얼 복원 전에 안전성 검사
	if (!OriginalMaterials.IsEmpty() || !OriginalSkeletalMaterials.IsEmpty())
	{
		RestoreOriginalMaterials();
	}
	else
	{
		// 맵이 비어있어도 무효한 참조가 있을 수 있으므로 정리
		CleanupInvalidReferences();
	}

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

	int32 StaticMeshApplied = 0;
	int32 SkeletalMeshApplied = 0;
	int32 ExcludedCount = 0;
	int32 GlassProtectedCount = 0;
	int32 TotalMaterialSlotsProcessed = 0;

	for (AActor* Actor : AllActors)
	{
		if (!Actor || !IsValid(Actor)) continue;

		if (ShouldExcludeActor(Actor))
		{
			ExcludedCount++;
			continue;
		}

		TArray<UStaticMeshComponent*> StaticMeshComponents;
		Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		for (UStaticMeshComponent* MeshComponent : StaticMeshComponents)
		{
			if (MeshComponent && IsValid(MeshComponent) && MeshComponent->GetStaticMesh())
			{
				int32 MaterialSlots = MeshComponent->GetNumMaterials();

				for (int32 SlotIndex = 0; SlotIndex < MaterialSlots; ++SlotIndex)
				{
					UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(SlotIndex);
					if (CurrentMaterial && IsValid(CurrentMaterial))
					{
						FString MaterialName = CurrentMaterial->GetName();
						
						if (MaterialName.Contains(TEXT("Glass"), ESearchCase::IgnoreCase) ||
							MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase) ||
							MaterialName.Contains(TEXT("M_Light"), ESearchCase::IgnoreCase) ||
							MaterialName.Contains(TEXT("MI_Light"), ESearchCase::IgnoreCase))
						{
							GlassProtectedCount++;
							continue;
						}
						
						if (!OriginalMaterials.Contains(MeshComponent))
						{
							// 추가 안전성 검사: 컴포넌트가 유효한지 확인
							if (MeshComponent && IsValid(MeshComponent))
							{
								OriginalMaterials.Add(MeshComponent, CurrentMaterial);
							}
						}

						UMaterialInterface* BasicShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
						if (BasicShapeMaterial)
						{
							MeshComponent->SetMaterial(SlotIndex, BasicShapeMaterial);
							TotalMaterialSlotsProcessed++;
						}
					}
				}

				StaticMeshApplied++;
			}
		}

		if (bIncludeSkeletalMeshes)
		{
			TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
			Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

			for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
			{
				if (MeshComponent && IsValid(MeshComponent) && MeshComponent->GetSkeletalMeshAsset())
				{
					int32 MaterialSlots = MeshComponent->GetNumMaterials();

					for (int32 SlotIndex = 0; SlotIndex < MaterialSlots; ++SlotIndex)
					{
						UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(SlotIndex);
						if (CurrentMaterial && IsValid(CurrentMaterial))
						{
							FString MaterialName = CurrentMaterial->GetName();
							
							if (MaterialName.Contains(TEXT("Glass"), ESearchCase::IgnoreCase) ||
								MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase) ||
								MaterialName.Contains(TEXT("M_Light"), ESearchCase::IgnoreCase))
							{
								GlassProtectedCount++;
								continue;
							}
							
							if (!OriginalSkeletalMaterials.Contains(MeshComponent))
							{
								// 추가 안전성 검사: 컴포넌트가 유효한지 확인
								if (MeshComponent && IsValid(MeshComponent))
								{
									OriginalSkeletalMaterials.Add(MeshComponent, CurrentMaterial);
								}
							}

							UMaterialInterface* BasicShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
							if (BasicShapeMaterial)
							{
								MeshComponent->SetMaterial(SlotIndex, BasicShapeMaterial);
								TotalMaterialSlotsProcessed++;
							}
						}
					}

					SkeletalMeshApplied++;
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Material Override: Applied to %d static, %d skeletal components. Protected %d glass materials."), 
		StaticMeshApplied, SkeletalMeshApplied, GlassProtectedCount);
}

void UMatrixMaterialOverrideSystem::RestoreOriginalMaterials()
{
	// 안전성 검사: 맵이 비어있으면 조기 반환
	if (OriginalMaterials.IsEmpty() && OriginalSkeletalMaterials.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("RestoreOriginalMaterials: No materials to restore"));
		return;
	}

	// 무효한 참조들을 미리 정리
	CleanupInvalidReferences();

	int32 RestoredStaticCount = 0;
	TArray<UStaticMeshComponent*> InvalidComponents;
	TArray<UStaticMeshComponent*> ValidComponents;
	TArray<UMaterialInterface*> ValidMaterials;

	// 먼저 유효한 컴포넌트들을 수집
	for (const auto& Pair : OriginalMaterials)
	{
		UStaticMeshComponent* MeshComponent = Pair.Key;
		UMaterialInterface* OriginalMaterial = Pair.Value;
		
		// 더 엄격한 유효성 검사
		if (MeshComponent && IsValid(MeshComponent) && 
			OriginalMaterial && IsValid(OriginalMaterial))
		{
			ValidComponents.Add(MeshComponent);
			ValidMaterials.Add(OriginalMaterial);
		}
		else
		{
			InvalidComponents.Add(MeshComponent);
		}
	}

	// 유효한 컴포넌트들에 대해서만 머티리얼 복원
	for (int32 i = 0; i < ValidComponents.Num(); ++i)
	{
		if (ValidComponents[i] && IsValid(ValidComponents[i]) && 
			ValidMaterials[i] && IsValid(ValidMaterials[i]))
		{
			ValidComponents[i]->SetMaterial(0, ValidMaterials[i]);
			RestoredStaticCount++;
		}
	}

	// 유효하지 않은 컴포넌트들을 맵에서 제거
	for (UStaticMeshComponent* InvalidComponent : InvalidComponents)
	{
		if (InvalidComponent)
		{
			OriginalMaterials.Remove(InvalidComponent);
		}
	}

	int32 RestoredSkeletalCount = 0;
	TArray<USkeletalMeshComponent*> InvalidSkeletalComponents;
	TArray<USkeletalMeshComponent*> ValidSkeletalComponents;
	TArray<UMaterialInterface*> ValidSkeletalMaterials;

	// 스켈레탈 메시 컴포넌트도 동일하게 처리
	for (const auto& Pair : OriginalSkeletalMaterials)
	{
		USkeletalMeshComponent* MeshComponent = Pair.Key;
		UMaterialInterface* OriginalMaterial = Pair.Value;
		
		// 더 엄격한 유효성 검사
		if (MeshComponent && IsValid(MeshComponent) && 
			OriginalMaterial && IsValid(OriginalMaterial))
		{
			ValidSkeletalComponents.Add(MeshComponent);
			ValidSkeletalMaterials.Add(OriginalMaterial);
		}
		else
		{
			InvalidSkeletalComponents.Add(MeshComponent);
		}
	}

	// 유효한 스켈레탈 컴포넌트들에 대해서만 머티리얼 복원
	for (int32 i = 0; i < ValidSkeletalComponents.Num(); ++i)
	{
		if (ValidSkeletalComponents[i] && IsValid(ValidSkeletalComponents[i]) && 
			ValidSkeletalMaterials[i] && IsValid(ValidSkeletalMaterials[i]))
		{
			ValidSkeletalComponents[i]->SetMaterial(0, ValidSkeletalMaterials[i]);
			RestoredSkeletalCount++;
		}
	}

	// 유효하지 않은 스켈레탈 컴포넌트들을 맵에서 제거
	for (USkeletalMeshComponent* InvalidComponent : InvalidSkeletalComponents)
	{
		if (InvalidComponent)
		{
			OriginalSkeletalMaterials.Remove(InvalidComponent);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Restored original materials for %d static components and %d skeletal components"), 
		RestoredStaticCount, RestoredSkeletalCount);
}

void UMatrixMaterialOverrideSystem::ApplyMaterialToActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* MeshComponent : StaticMeshComponents)
	{
		if (MeshComponent && IsValid(MeshComponent) && MeshComponent->GetStaticMesh())
		{
			if (!OriginalMaterials.Contains(MeshComponent))
			{
				// 추가 안전성 검사
				if (MeshComponent && IsValid(MeshComponent))
				{
					UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(0);
					if (CurrentMaterial && IsValid(CurrentMaterial))
					{
						OriginalMaterials.Add(MeshComponent, CurrentMaterial);
					}
				}
			}

			UMaterialInterface* BasicShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
			if (BasicShapeMaterial)
			{
				ApplyMaterialToStaticMeshComponent(MeshComponent, BasicShapeMaterial);
			}
		}
	}

	if (bIncludeSkeletalMeshes)
	{
		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

		for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
		{
					if (MeshComponent && IsValid(MeshComponent) && MeshComponent->GetSkeletalMeshAsset())
		{
			if (!OriginalSkeletalMaterials.Contains(MeshComponent))
			{
				// 추가 안전성 검사
				if (MeshComponent && IsValid(MeshComponent))
				{
					UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(0);
					if (CurrentMaterial && IsValid(CurrentMaterial))
					{
						OriginalSkeletalMaterials.Add(MeshComponent, CurrentMaterial);
					}
				}
			}

				UMaterialInterface* BasicShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
				if (BasicShapeMaterial)
				{
					ApplyMaterialToSkeletalMeshComponent(MeshComponent, BasicShapeMaterial);
				}
			}
		}
	}
}

bool UMatrixMaterialOverrideSystem::ShouldExcludeActor(AActor* Actor) const
{
	if (!Actor || ExcludeTags.IsEmpty())
	{
		return false;
	}

	for (const FString& ExcludeTag : ExcludeTags)
	{
		if (Actor->ActorHasTag(FName(*ExcludeTag)))
		{
			return true;
		}
	}

	FString ActorName = Actor->GetName();
	for (const FString& ExcludeTag : ExcludeTags)
	{
		if (ActorName.Contains(ExcludeTag, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}

	return false;
}

void UMatrixMaterialOverrideSystem::ApplyMaterialToStaticMeshComponent(UStaticMeshComponent* MeshComponent, UMaterialInterface* Material)
{
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyMaterialToStaticMeshComponent: MeshComponent is null"));
		return;
	}
	
	if (!Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyMaterialToStaticMeshComponent: Material is null"));
		return;
	}

	MeshComponent->SetMaterial(0, Material);
	
}

void UMatrixMaterialOverrideSystem::ApplyMaterialToSkeletalMeshComponent(USkeletalMeshComponent* MeshComponent, UMaterialInterface* Material)
{
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyMaterialToSkeletalMeshComponent: MeshComponent is null"));
		return;
	}
	
	if (!Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyMaterialToSkeletalMeshComponent: Material is null"));
		return;
	}

	MeshComponent->SetMaterial(0, Material);
	
}

int32 UMatrixMaterialOverrideSystem::GetAppliedMaterialCount() const
{
	return OriginalMaterials.Num() + OriginalSkeletalMaterials.Num();
}

bool UMatrixMaterialOverrideSystem::IsSystemActive() const
{
	return !OriginalMaterials.IsEmpty() || !OriginalSkeletalMaterials.IsEmpty();
}

void UMatrixMaterialOverrideSystem::ApplyWhiteMaterialWithDelay(float DelaySeconds)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	if (DelayTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(DelayTimerHandle);
	}

	World->GetTimerManager().SetTimer(DelayTimerHandle, this, &UMatrixMaterialOverrideSystem::DelayedApplyWhiteMaterial, DelaySeconds, false);
}

void UMatrixMaterialOverrideSystem::DelayedApplyWhiteMaterial()
{
	ApplyWhiteMaterialToAllMeshes();
	
}

void UMatrixMaterialOverrideSystem::ApplyMaterialToActorsByNamePattern(const FString& NamePattern)
{
	// WhiteMaterial 로직 삭제됨 - BasicShapeMaterial 사용

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

	int32 MatchedActors = 0;
	int32 TotalMaterialSlotsProcessed = 0;

	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;

		FString ActorName = Actor->GetName();
		
		if (ActorName.Contains(NamePattern, ESearchCase::IgnoreCase))
		{
			MatchedActors++;

			if (ShouldExcludeActor(Actor))
			{
				continue;
			}

			TArray<UStaticMeshComponent*> StaticMeshComponents;
			Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

			for (UStaticMeshComponent* MeshComponent : StaticMeshComponents)
			{
				if (MeshComponent && MeshComponent->GetStaticMesh() && IsValid(MeshComponent))
				{
					int32 MaterialSlots = MeshComponent->GetNumMaterials();
					for (int32 SlotIndex = 0; SlotIndex < MaterialSlots; ++SlotIndex)
					{
						UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(SlotIndex);
						if (CurrentMaterial && IsValid(CurrentMaterial))
						{
							FString MaterialName = CurrentMaterial->GetName();
							
							if (MaterialName.Contains(TEXT("Glass"), ESearchCase::IgnoreCase) ||
								MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase) ||
								MaterialName.Contains(TEXT("M_Light"), ESearchCase::IgnoreCase) ||
								MaterialName.Contains(TEXT("MI_Light"), ESearchCase::IgnoreCase))
							{
								continue;
							}
							
							if (!OriginalMaterials.Contains(MeshComponent))
							{
								OriginalMaterials.Add(MeshComponent, CurrentMaterial);
							}

							UMaterialInterface* BasicShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
							if (BasicShapeMaterial)
							{
								MeshComponent->SetMaterial(SlotIndex, BasicShapeMaterial);
								TotalMaterialSlotsProcessed++;
							}
						}
					}
				}
			}

			if (bIncludeSkeletalMeshes)
			{
				TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
				Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

				for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
				{
					if (MeshComponent && MeshComponent->GetSkeletalMeshAsset() && IsValid(MeshComponent))
					{
						int32 MaterialSlots = MeshComponent->GetNumMaterials();
						for (int32 SlotIndex = 0; SlotIndex < MaterialSlots; ++SlotIndex)
						{
							UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(SlotIndex);
							if (CurrentMaterial && IsValid(CurrentMaterial))
							{
								FString MaterialName = CurrentMaterial->GetName();
								
								if (MaterialName.Contains(TEXT("Glass"), ESearchCase::IgnoreCase) ||
									MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase) ||
									MaterialName.Contains(TEXT("M_Light"), ESearchCase::IgnoreCase) ||
									MaterialName.Contains(TEXT("MI_Light"), ESearchCase::IgnoreCase))
								{
									continue;
								}
								
								if (!OriginalSkeletalMaterials.Contains(MeshComponent))
								{
									OriginalSkeletalMaterials.Add(MeshComponent, CurrentMaterial);
								}

								UMaterialInterface* BasicShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
								if (BasicShapeMaterial)
								{
									MeshComponent->SetMaterial(SlotIndex, BasicShapeMaterial);
									TotalMaterialSlotsProcessed++;
								}
							}
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Name Pattern: %s, Matched actors: %d, Total material slots processed: %d"), 
		*NamePattern, MatchedActors, TotalMaterialSlotsProcessed);
}

void UMatrixMaterialOverrideSystem::StartRepeatingMaterialApplication(float IntervalSeconds, int32 MaxAttempts)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	if (RepeatingTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(RepeatingTimerHandle);
	}

	CurrentAttempts = 0;
	MaxRepeatingAttempts = MaxAttempts;
	RepeatingInterval = IntervalSeconds;

	World->GetTimerManager().SetTimer(RepeatingTimerHandle, this, &UMatrixMaterialOverrideSystem::RepeatingApplyWhiteMaterial, 
		IntervalSeconds, true);
	
}

void UMatrixMaterialOverrideSystem::StopRepeatingMaterialApplication()
{
	UWorld* World = GetWorld();
	if (World && RepeatingTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(RepeatingTimerHandle);
	}
}

void UMatrixMaterialOverrideSystem::RepeatingApplyWhiteMaterial()
{
	CurrentAttempts++;
	UWorld* World = GetWorld();
	if (!World || !IsValid(World))
	{
		StopRepeatingMaterialApplication();
		return;
	}

	if (CurrentAttempts >= MaxRepeatingAttempts)
	{
		StopRepeatingMaterialApplication();
		return;
	}

	ApplyWhiteMaterialToAllMeshes();
	
}

void UMatrixMaterialOverrideSystem::DebugPrintAllActors() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;

		FString ActorName = Actor->GetName();
		FString ClassName = Actor->GetClass()->GetName();
		
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
		
		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

		UE_LOG(LogTemp, Log, TEXT("Actor: %s (Class: %s) - StaticMesh: %d, SkeletalMesh: %d"), 
			*ActorName, *ClassName, StaticMeshComponents.Num(), SkeletalMeshComponents.Num());
	}

}

void UMatrixMaterialOverrideSystem::DebugPrintActorMaterials(const FString& ActorName) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;

		if (Actor->GetName().Contains(ActorName, ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Log, TEXT("Found matching actor: %s (Class: %s)"), 
				*Actor->GetName(), *Actor->GetClass()->GetName());

			TArray<UStaticMeshComponent*> StaticMeshComponents;
			Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

			for (UStaticMeshComponent* MeshComponent : StaticMeshComponents)
			{
				if (MeshComponent && MeshComponent->GetStaticMesh())
				{
					UE_LOG(LogTemp, Log, TEXT("  StaticMesh Component: %s (Mesh: %s)"), 
						*MeshComponent->GetName(), *MeshComponent->GetStaticMesh()->GetName());

					int32 MaterialSlots = MeshComponent->GetNumMaterials();
					for (int32 SlotIndex = 0; SlotIndex < MaterialSlots; ++SlotIndex)
					{
						UMaterialInterface* Material = MeshComponent->GetMaterial(SlotIndex);
						if (Material)
						{
							UE_LOG(LogTemp, Log, TEXT("    Slot %d: %s"), SlotIndex, *Material->GetName());
						}
						else
						{
							UE_LOG(LogTemp, Log, TEXT("    Slot %d: No material"), SlotIndex);
						}
					}
				}
			}

			TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
			Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

			for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
			{
				if (MeshComponent && MeshComponent->GetSkeletalMeshAsset())
				{
					UE_LOG(LogTemp, Log, TEXT("  SkeletalMesh Component: %s (Mesh: %s)"), 
						*MeshComponent->GetName(), *MeshComponent->GetSkeletalMeshAsset()->GetName());

					int32 MaterialSlots = MeshComponent->GetNumMaterials();
					for (int32 SlotIndex = 0; SlotIndex < MaterialSlots; ++SlotIndex)
					{
						UMaterialInterface* Material = MeshComponent->GetMaterial(SlotIndex);
						if (Material)
						{
							UE_LOG(LogTemp, Log, TEXT("    Slot %d: %s"), SlotIndex, *Material->GetName());
						}
						else
						{
							UE_LOG(LogTemp, Log, TEXT("    Slot %d: No material"), SlotIndex);
						}
					}
				}
			}
		}
	}

}

void UMatrixMaterialOverrideSystem::CleanupInvalidReferences()
{
	int32 RemovedStaticCount = 0;
	int32 RemovedSkeletalCount = 0;

	// 무효한 스태틱 메시 컴포넌트 참조 제거
	TArray<UStaticMeshComponent*> InvalidStaticComponents;
	for (const auto& Pair : OriginalMaterials)
	{
		UStaticMeshComponent* MeshComponent = Pair.Key;
		if (!MeshComponent || !IsValid(MeshComponent))
		{
			InvalidStaticComponents.Add(MeshComponent);
		}
	}

	for (UStaticMeshComponent* InvalidComponent : InvalidStaticComponents)
	{
		if (InvalidComponent)
		{
			OriginalMaterials.Remove(InvalidComponent);
			RemovedStaticCount++;
		}
	}

	// 무효한 스켈레탈 메시 컴포넌트 참조 제거
	TArray<USkeletalMeshComponent*> InvalidSkeletalComponents;
	for (const auto& Pair : OriginalSkeletalMaterials)
	{
		USkeletalMeshComponent* MeshComponent = Pair.Key;
		if (!MeshComponent || !IsValid(MeshComponent))
		{
			InvalidSkeletalComponents.Add(MeshComponent);
		}
	}

	for (USkeletalMeshComponent* InvalidComponent : InvalidSkeletalComponents)
	{
		if (InvalidComponent)
		{
			OriginalSkeletalMaterials.Remove(InvalidComponent);
			RemovedSkeletalCount++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("CleanupInvalidReferences: Removed %d static and %d skeletal invalid references"), 
		RemovedStaticCount, RemovedSkeletalCount);
}
