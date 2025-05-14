// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindPatrolPos();

	//비헤비어 트리는 UBTTAskNode를 실행할때 ExecuteTask를 실행하며, 반환형이 되는 EBTNodeResult::Type은 아래 총 네개이다.
	//Aborted: 태스크 실행중 중단. 결과적으로 실패
	//Faile: 태스크를 수행했지만 실패했다.
	//Succeeded: 태스크를 성공적으로 수행했다.
	//InProgress: 태스크를 계속 수행하고 있다. 실행결과는 향후 알려줄 예정.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
