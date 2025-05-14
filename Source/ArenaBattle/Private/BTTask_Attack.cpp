// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack() {
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter) return EBTNodeResult::Failed;

	ABCharacter->Attack(FInputActionValue());
	IsAttacking = true;
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
		IsAttacking = false;
		});

	//���� Task�� ���� �ִϸ��̼��� ���������� ��ٷ����ϴ� �׽�ũ�̹Ƿ� �ϴ� InProgress�� ��ȯ
	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking) {
		//FInishLatentTask�� Ʈ�� �ý��ۿ� �� �׽�ũ�� �����ٰ� �˷��ִ� �Լ��̴�.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}