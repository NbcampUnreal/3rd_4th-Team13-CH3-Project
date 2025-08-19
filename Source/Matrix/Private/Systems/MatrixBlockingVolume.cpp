#include "Systems/MatrixBlockingVolume.h"
#include "GameFramework/MatrixLevelManager.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

AMatrixBlockingVolume::AMatrixBlockingVolume()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // BoxComponent를 사용하여 물리적 충돌 생성
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;
    
    // 물리적 충돌 활성화
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxComponent->SetCollisionResponseToAllChannels(ECR_Block);
    BoxComponent->SetCollisionObjectType(ECC_WorldStatic);
    
    // 기본 크기 설정
    BoxComponent->SetBoxExtent(FVector(100.0f, 100.0f, 200.0f));
    
    // 디버그용으로 보이게 설정 (나중에 숨길 수 있음)
    BoxComponent->SetVisibility(true);
    BoxComponent->SetHiddenInGame(false);
}

void AMatrixBlockingVolume::BeginPlay()
{
    Super::BeginPlay();
    
    if (bAutoRegister)
    {
        // 레벨 매니저에 자동 등록
        UMatrixLevelManager* LevelManager = GetGameInstance()->GetSubsystem<UMatrixLevelManager>();
        if (LevelManager)
        {
            LevelManager->RegisterFloorBlockingVolume(FloorNumber, this);
            UE_LOG(LogTemp, Log, TEXT("MatrixBlockingVolume %s auto-registered for floor %d"), *GetName(), FloorNumber);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LevelManager not found for auto-registration of MatrixBlockingVolume %s"), *GetName());
        }
    }
}

void AMatrixBlockingVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
