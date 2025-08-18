#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotHUDWidget.generated.h"

class ABaseItem;
class UImage;
class UTextBlock;
class UBorder;

UCLASS()
class MATRIX_API USlotHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void UpdateQuickSlots(const TArray<ABaseItem*>& Items);

protected:                                                                
	UPROPERTY(meta = (BindWidget)) 
	UImage* Img_Icon01 = nullptr;     
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* Text_Count01 = nullptr;
	
	UPROPERTY(meta = (BindWidget)) 
	UImage* Img_Icon02 = nullptr;   
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* Text_Count02 = nullptr;
	
	UPROPERTY(meta = (BindWidget)) 
	UImage* Img_Icon03 = nullptr;  
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* Text_Count03 = nullptr;	
};
