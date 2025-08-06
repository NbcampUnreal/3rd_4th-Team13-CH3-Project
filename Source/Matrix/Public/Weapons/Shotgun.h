#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem/WeaponBase.h"
#include "Shotgun.generated.h"

UCLASS()
class MATRIX_API AShotgun : public AWeaponBase
{
	GENERATED_BODY()

public:
	AShotgun();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun")
	int32 NumBulletsPerShot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun")
	float SpreadAngle;

protected:
	virtual bool FireBullet() override;
};
