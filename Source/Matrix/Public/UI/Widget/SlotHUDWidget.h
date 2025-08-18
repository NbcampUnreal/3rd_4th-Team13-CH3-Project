#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotHUDWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;

UCLASS()
class MATRIX_API USlotHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	

protected:                                                                

	// UPROPERTY(meta = (BindWidget)) 
	// UImage* Img_Icon01 = nullptr;  
	// 1번칸 아이콘 이미지 -> 아이콘 이미지를 고정하지 않고 얻는 순서에 따라 다르게 하시려면 사용하세요.      
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* Text_Count01 = nullptr; // 1번칸 아이템의 개수
	// UPROPERTY(meta = (BindWidget)) 
	// UImage* Img_Icon02 = nullptr;  
	// 2번칸 아이콘 이미지 -> 아이콘 이미지를 고정하지 않고 얻는 순서에 따라 다르게 하시려면 사용하세요.      
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* Text_Count02 = nullptr;	// 1번칸 아이템의 개수
	// UPROPERTY(meta = (BindWidget)) 
	// UImage* Img_Icon03 = nullptr;  
	// 3번칸 아이콘 이미지 -> 아이콘 이미지를 고정하지 않고 얻는 순서에 따라 다르게 하시려면 사용하세요.      
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* Text_Count03 = nullptr;	// 1번칸 아이템의 개수
	

};
