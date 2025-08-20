#include "GameFramework/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "kismet/GameplayStatics.h"

UInventoryComponent::UInventoryComponent()
{
	MaxSlots = 3;
	Items.Init(nullptr, MaxSlots);
}

//아이템 추가 함수
void UInventoryComponent::AddItem(ABaseItem* NewItem)
{
	if (!NewItem) return;
	
	PlaySound();

	for (ABaseItem* CurrentItem : Items)
	{
		//현재 아이템 타입 = 주운 아이템 타입일 경우
		if (CurrentItem && CurrentItem->GetItemType() == NewItem->GetItemType()) 
		{
			CurrentItem->SetItemCount(CurrentItem->GetItemCount() + 1);
			NewItem->Destroy();
			OnInventoryUpdated.Broadcast();
			return;
		}
	}
	
	for (int32 i = 0; i < Items.Num(); i++)
	{
		//신규 아이템 + 인벤토리 공간이 비어있을 경우
		if (Items[i] == nullptr)
		{
			Items[i] = NewItem;
			NewItem->PickedUpItem();
			OnInventoryUpdated.Broadcast();
			return;
		}
	}
}

//아이템 제거 함수
void UInventoryComponent::RemoveItem(int32 Index)
{
	if (Items.IsValidIndex(Index) && Items[Index])
	{
		//아이템 사용 함수 호출
		Items[Index]->UseItem();
		//아이템 개수가 0일 경우 인벤토리에서 제거
		if (Items[Index]->GetItemCount() <= 0)
			Items[Index] = nullptr;
	}
	
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::PlaySound()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, PlayerPawn->GetActorLocation());
}