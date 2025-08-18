#include "UI/Widget/SlotHUDWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/BaseItem.h"

void USlotHUDWidget::UpdateQuickSlots(const TArray<ABaseItem*>& Items)
{
	const int32 NumSlots = 3;

	TArray<UImage*> IconSlots = { Img_Icon01, Img_Icon02, Img_Icon03 };
	TArray<UTextBlock*> CountSlots = { Text_Count01, Text_Count02, Text_Count03 };

	for (int32 i = 0; i < NumSlots; i++)
	{
		if (i < Items.Num() && Items[i] != nullptr)
		{
			ABaseItem* Item = Items[i];

			if (Item->GetItemIcon() != nullptr)
			{
				IconSlots[i]->SetBrushFromTexture(Item->GetItemIcon());
				IconSlots[i]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				IconSlots[i]->SetVisibility(ESlateVisibility::Hidden);
			}

			int32 ItemCount = Item->GetItemCount();
			CountSlots[i]->SetText(FText::AsNumber(ItemCount));
			CountSlots[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			IconSlots[i]->SetVisibility(ESlateVisibility::Hidden);
			CountSlots[i]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
