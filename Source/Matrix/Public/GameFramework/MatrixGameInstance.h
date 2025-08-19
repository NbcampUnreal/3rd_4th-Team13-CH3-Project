#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MatrixGameInstance.generated.h"

class UMatrixMaterialOverrideSystem;

UCLASS()
class MATRIX_API UMatrixGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Data")
	int32 PlayerScore = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Game Data")
	int32 CurrentLevel = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Game Data")
	int32 MaxLevel = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Options")
	float SavedSensitivity = 0.5f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Options")
	float SavedBrightness = 0.5f;

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void AddScore(int32 ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void ResetGameData();

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void AdvanceToNextLevel();

	// 머티리얼 오버라이드 시스템
	UPROPERTY(BlueprintReadOnly, Category = "Material Override")
	UMatrixMaterialOverrideSystem* MaterialOverrideSystem;

	// 머티리얼 오버라이드 시스템 초기화
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void InitializeMaterialOverrideSystem();

	// 하얀색 머티리얼 적용 (기존 방식)
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyWhiteMaterialToWorld();

	// 하얀색 머티리얼 적용 (모든 메시 컴포넌트)
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyWhiteMaterialToAllMeshes();

	// 하얀색 머티리얼 적용 (지연 실행)
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyWhiteMaterialWithDelay(float DelaySeconds = 2.0f);

	// 반복 실행으로 머티리얼 적용 (스트리밍 레벨용)
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void StartRepeatingMaterialApplication(float IntervalSeconds = 5.0f, int32 MaxAttempts = 10);

	// 반복 실행 중지
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void StopRepeatingMaterialApplication();

	// 특정 이름 패턴의 액터들에 머티리얼 적용
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void ApplyMaterialToActorsByNamePattern(const FString& NamePattern);

	// 디버그: 모든 액터 정보 출력
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void DebugPrintAllActors() const;

	// 디버그: 특정 액터의 머티리얼 정보 출력
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void DebugPrintActorMaterials(const FString& ActorName) const;

	// 원래 머티리얼로 복원
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void RestoreOriginalMaterials();

	// 무효한 참조 정리
	UFUNCTION(BlueprintCallable, Category = "Material Override")
	void CleanupInvalidReferences();
};
