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

	//������ Ʈ���� UBTTAskNode�� �����Ҷ� ExecuteTask�� �����ϸ�, ��ȯ���� �Ǵ� EBTNodeResult::Type�� �Ʒ� �� �װ��̴�.
	//Aborted: �½�ũ ������ �ߴ�. ��������� ����
	//Faile: �½�ũ�� ���������� �����ߴ�.
	//Succeeded: �½�ũ�� ���������� �����ߴ�.
	//InProgress: �½�ũ�� ��� �����ϰ� �ִ�. �������� ���� �˷��� ����.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
