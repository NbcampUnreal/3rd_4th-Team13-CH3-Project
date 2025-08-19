#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MatrixMaterialOverrideSystem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MATRIX_API UMatrixMaterialOverrideSystem : public UObject
{
	GENERATED_BODY()

public:
	UMatrixMaterialOverrideSystem();

	// WhiteMaterial 변수 삭제됨 - BasicShapeMaterial 사용

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Override")
	UMaterialInterface* GlassMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Override")
	TArray<FString> ExcludeTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Override")
	bool bIncludeSkeletalMeshes = false;

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyWhiteMaterialToWorld();

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyWhiteMaterialToAllMeshes();

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void RestoreOriginalMaterials();

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyMaterialToActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	bool ShouldExcludeActor(AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	int32 GetAppliedMaterialCount() const;

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	bool IsSystemActive() const;

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyWhiteMaterialWithDelay(float DelaySeconds = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void StartRepeatingMaterialApplication(float IntervalSeconds = 5.0f, int32 MaxAttempts = 10);

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void StopRepeatingMaterialApplication();

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyMaterialToActorsByNamePattern(const FString& NamePattern);

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void DebugPrintAllActors() const;

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void DebugPrintActorMaterials(const FString& ActorName) const;

	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void CleanupInvalidReferences();

private:
	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, TObjectPtr<UMaterialInterface>> OriginalMaterials;

	UPROPERTY()
	TMap<TObjectPtr<USkeletalMeshComponent>, TObjectPtr<UMaterialInterface>> OriginalSkeletalMaterials;

	void ApplyMaterialToStaticMeshComponent(UStaticMeshComponent* MeshComponent, UMaterialInterface* Material);
	void ApplyMaterialToSkeletalMeshComponent(USkeletalMeshComponent* MeshComponent, UMaterialInterface* Material);

	FTimerHandle DelayTimerHandle;
	FTimerHandle RepeatingTimerHandle;
	int32 CurrentAttempts = 0;
	int32 MaxRepeatingAttempts = 10;
	float RepeatingInterval = 5.0f;

	void DelayedApplyWhiteMaterial();
	void RepeatingApplyWhiteMaterial();
};
