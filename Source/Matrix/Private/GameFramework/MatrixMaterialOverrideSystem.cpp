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
	if (!WhiteMaterial)
	{
		WhiteMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (!WhiteMaterial)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load default white material!"));
			return;
		}
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is not available!"));
		return;
	}

	if (!OriginalMaterials.IsEmpty() || !OriginalSkeletalMaterials.IsEmpty())
	{
		RestoreOriginalMaterials();
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
		if (!Actor) continue;

		if (ShouldExcludeActor(Actor))
		{
			ExcludedCount++;
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
							MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase))
						{
							GlassProtectedCount++;
							continue;
						}
						
						if (!OriginalMaterials.Contains(MeshComponent))
						{
							OriginalMaterials.Add(MeshComponent, CurrentMaterial);
						}

						MeshComponent->SetMaterial(SlotIndex, WhiteMaterial);
						TotalMaterialSlotsProcessed++;
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
								MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase))
							{
								GlassProtectedCount++;
								continue;
							}
							
							if (!OriginalSkeletalMaterials.Contains(MeshComponent))
							{
								OriginalSkeletalMaterials.Add(MeshComponent, CurrentMaterial);
							}

							MeshComponent->SetMaterial(SlotIndex, WhiteMaterial);
							TotalMaterialSlotsProcessed++;
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
	int32 RestoredStaticCount = 0;
	TArray<UStaticMeshComponent*> InvalidComponents;

	for (const auto& Pair : OriginalMaterials)
	{
		UStaticMeshComponent* MeshComponent = Pair.Key;
		UMaterialInterface* OriginalMaterial = Pair.Value;
		
		if (IsValid(MeshComponent) && IsValid(OriginalMaterial))
		{
			MeshComponent->SetMaterial(0, OriginalMaterial);
			RestoredStaticCount++;
		}
		else
		{
			InvalidComponents.Add(MeshComponent);
		}
	}

	for (UStaticMeshComponent* InvalidComponent : InvalidComponents)
	{
		OriginalMaterials.Remove(InvalidComponent);
	}

	int32 RestoredSkeletalCount = 0;
	TArray<USkeletalMeshComponent*> InvalidSkeletalComponents;

	for (const auto& Pair : OriginalSkeletalMaterials)
	{
		USkeletalMeshComponent* MeshComponent = Pair.Key;
		UMaterialInterface* OriginalMaterial = Pair.Value;
		
		if (IsValid(MeshComponent) && IsValid(OriginalMaterial))
		{
			MeshComponent->SetMaterial(0, OriginalMaterial);
			RestoredSkeletalCount++;
		}
		else
		{
			InvalidSkeletalComponents.Add(MeshComponent);
		}
	}

	for (USkeletalMeshComponent* InvalidComponent : InvalidSkeletalComponents)
	{
		OriginalSkeletalMaterials.Remove(InvalidComponent);
	}

	UE_LOG(LogTemp, Log, TEXT("Restored original materials for %d static components and %d skeletal components"), 
		RestoredStaticCount, RestoredSkeletalCount);
}

void UMatrixMaterialOverrideSystem::ApplyMaterialToActor(AActor* Actor)
{
	if (!Actor || !WhiteMaterial)
	{
		return;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* MeshComponent : StaticMeshComponents)
	{
		if (MeshComponent && MeshComponent->GetStaticMesh())
		{
			if (!OriginalMaterials.Contains(MeshComponent))
			{
				OriginalMaterials.Add(MeshComponent, MeshComponent->GetMaterial(0));
			}

			ApplyMaterialToStaticMeshComponent(MeshComponent, WhiteMaterial);
		}
	}

	if (bIncludeSkeletalMeshes)
	{
		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

		for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
		{
			if (MeshComponent && MeshComponent->GetSkeletalMeshAsset())
			{
				if (!OriginalSkeletalMaterials.Contains(MeshComponent))
				{
					OriginalSkeletalMaterials.Add(MeshComponent, MeshComponent->GetMaterial(0));
				}

				ApplyMaterialToSkeletalMeshComponent(MeshComponent, WhiteMaterial);
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
	if (!WhiteMaterial)
	{
		WhiteMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (!WhiteMaterial)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load default white material!"));
			return;
		}
	}

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
								MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase))
							{
								continue;
							}
							
							if (!OriginalMaterials.Contains(MeshComponent))
							{
								OriginalMaterials.Add(MeshComponent, CurrentMaterial);
							}

							MeshComponent->SetMaterial(SlotIndex, WhiteMaterial);
							TotalMaterialSlotsProcessed++;
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
									MaterialName.Contains(TEXT("glass"), ESearchCase::IgnoreCase))
								{
									continue;
								}
								
								if (!OriginalSkeletalMaterials.Contains(MeshComponent))
								{
									OriginalSkeletalMaterials.Add(MeshComponent, CurrentMaterial);
								}

								MeshComponent->SetMaterial(SlotIndex, WhiteMaterial);
								TotalMaterialSlotsProcessed++;
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
