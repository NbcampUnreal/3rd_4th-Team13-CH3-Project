#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "MatrixBlockingVolume.generated.h"

UCLASS()
class MATRIX_API AMatrixBlockingVolume : public AActor
{
    GENERATED_BODY()
    
public:    
    AMatrixBlockingVolume();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;
    
    // 이 Blocking Volume이 속한 층
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Settings")
    int32 FloorNumber = 2;
    
    // 자동 등록 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
    bool bAutoRegister = true;
    
private:
    UPROPERTY()
    UBoxComponent* BoxComponent;
};
