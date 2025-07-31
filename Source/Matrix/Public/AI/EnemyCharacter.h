// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class MATRIX_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	UPROPERTY(EditAnywhere, Category = "AI")
	float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
	float RunSpeed = 600.0f;

	void SetMovementSpeed(float NewSpeed);
	
protected:
	virtual void BeginPlay() override;
};
