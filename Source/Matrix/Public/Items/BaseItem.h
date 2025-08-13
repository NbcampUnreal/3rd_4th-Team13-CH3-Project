#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class MATRIX_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	void PickedUpItem();
	int32 GetItemCount() const {return ItemCount;};
	UTexture2D* GetItemIcon() const {return ItemIcon;};
	FName GetItemType() const {return ItemType;};
	void SetItemCount(int32 Count);
	
	virtual void UseItem();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
	bool bIsPickedUp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ActivateParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ActivateSound;
};
