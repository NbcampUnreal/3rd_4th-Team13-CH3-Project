#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletPoolManager.generated.h"

class ABulletBase;

UCLASS()
class MATRIX_API ABulletPoolManager : public AActor
{
	GENERATED_BODY()

public:
	ABulletPoolManager();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Bullet")
	TArray<TSubclassOf<ABulletBase>> BulletClasses;
	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 BulletPoolSize = 100;

	UFUNCTION(BlueprintCallable, Category = "Bullet")
	ABulletBase* GetBullet(TSubclassOf<ABulletBase> BulletClass);

private:
	TMap<TSubclassOf<ABulletBase>, TArray<ABulletBase*>> BulletPool;

	void CreateBulletPool();
};
