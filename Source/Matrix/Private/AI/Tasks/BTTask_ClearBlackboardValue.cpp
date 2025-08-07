#include "AI/Tasks/BTTask_ClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue()
{
	NodeName = TEXT("Clear Blackboard Value");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey), UObject::StaticClass());
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey));
	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey));
	BlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey));
	BlackboardKey.AddStringFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey));
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey));
	BlackboardKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearBlackboardValue, BlackboardKey));
}

EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp && BlackboardKey.SelectedKeyName != NAME_None)
	{
		BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
