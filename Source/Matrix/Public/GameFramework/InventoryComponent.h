#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

class ABaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MATRIX_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	
	void AddItem(ABaseItem* NewItem); //아이템 추가 함수
	void RemoveItem(int32 Index); //아이템 제거 함수
	ABaseItem* GetItems(int32 Slot) const { return Items.IsValidIndex(Slot) ? Items[Slot] : nullptr; } //캐릭터에서 퀵슬롯 정보 get
	TArray<ABaseItem*> GetAllItems() const { return Items; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ABaseItem*> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* PickupSound;

	void PlaySound();
};
