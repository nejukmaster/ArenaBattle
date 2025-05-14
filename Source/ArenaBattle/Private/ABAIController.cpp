// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController() {
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	if (BBObject.Succeeded()) {
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
	if (BTObject.Succeeded()) {
		BTAsset = BTObject.Object;
		ABLOG_S(Warning);
	}
}

void AABAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	/*
	if (UseBlackboard(BBAsset, blackboard)) {
		
	}*/
	//GetWorld()->GetTimerManager().SetTimer(RepeateTimerHandle, this, &AABAIController::OnRepeatTimer, RepeatInterval, true);
}

void AABAIController::RunAI() {
	AAIController::RunBehaviorTree(BTAsset);
	GetBlackboardComponent()->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());
	if (!RunBehaviorTree(BTAsset)) {
		ABLOG(Error, TEXT("AIController couldn't run behavior tree!"));
	}
}

void AABAIController::StopAI() {
	//BrainComponent는 AIController에서 동작을 담당하는 컴포넌트를 반환합니다.
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr != BehaviorTreeComponent) {
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

/*
void AABAIController::OnUnPossess() {
	Super::OnUnPossess();
	GetWorld()->GetTimerManager().ClearTimer(RepeateTimerHandle);
}
*/



