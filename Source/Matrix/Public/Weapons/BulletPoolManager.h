#pragma once

#include "CoreMinimal.h"
#include "BulletPoolManager.generated.h"

class ABulletBase;

UCLASS()
class MATRIX_API UBulletPoolManager : public UObject
{
	GENERATED_BODY()

public:
	UBulletPoolManager();
	void Init();

	UPROPERTY(EditAnywhere, Category = "Bullet")
	TSubclassOf<ABulletBase> BulletClass;
	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 BulletPoolSize = 30;

	UFUNCTION(BlueprintCallable)
	ABulletBase* GetBullet();

private:
	UPROPERTY()
	TArray<ABulletBase*> BulletPool;

	void CreateBulletPool();
};
