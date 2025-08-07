#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MatrixAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MATRIX_API UMatrixAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Declare your animation blueprint variables here as UPROPERTY(BlueprintReadWrite)
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsAttacking;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsEvading;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsTakingCover;

	// Add other animation-related variables as needed

	// Anim Notify 콜백 함수 선언
	UFUNCTION()
	void AnimNotify_AttackHit(); // 공격 히트 시 호출될 함수

	UFUNCTION()
	void AnimNotify_AbilityEnd(); // 어빌리티 애니메이션 종료 시 호출될 함수
};
