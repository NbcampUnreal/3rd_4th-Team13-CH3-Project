#include "AI/Decorators/BTDecorator_CheckDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"

UBTDecorator_CheckDistance::UBTDecorator_CheckDistance()
{
    NodeName = TEXT("Check Distance");
    TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistance, TargetLocationKey));
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistance, TargetActorKey), AActor::StaticClass());
}

bool UBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return false;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return false;
    
    // 블랙보드에서 목표 위치 가져오기
    FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);
    
    // AI의 현재 위치 가져오기
    FVector AILocation = AIPawn->GetActorLocation();

    // --- Z축을 발밑 위치로 조정 ---
    // AI 자신의 발밑 위치 계산
    ACharacter* AICharacter = Cast<ACharacter>(AIPawn);
    if (AICharacter && AICharacter->GetCapsuleComponent())
    {
        AILocation.Z -= AICharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    }

    // 목표(플레이어)의 발밑 위치 계산
    // TargetActorKey는 UPROPERTY로 선언된 멤버 변수이므로 직접 접근합니다.
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    float TargetHalfHeight = 0.0f;
    if (TargetActor)
    {
        ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
        if (TargetCharacter && TargetCharacter->GetCapsuleComponent())
        {
            TargetHalfHeight = TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
        }
        else if (TargetActor->GetRootComponent()) // ACharacter가 아니거나 캡슐 컴포넌트가 없는 경우
        {
            TargetHalfHeight = TargetActor->GetRootComponent()->Bounds.BoxExtent.Z;
        }
    }
    TargetLocation.Z -= TargetHalfHeight;
    TargetLocation.Z = AILocation.Z;
    // --- Z축 조정 끝 ---
    
    float Distance = FVector::Dist(AILocation, TargetLocation);

    // 조건 반환
    if (bCheckWithinRange)
    {
        return Distance <= DistanceThreshold;
    }
    else
    {
        return Distance > DistanceThreshold;
    }
}




