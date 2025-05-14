// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect() {
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;
	
	if (nullptr == World)return;

	//충돌 검사시 결과를 담을 FOvelapResult의 TArray배열
	//TArray는 언리얼에서 미리 구현해놓은 동적 배열이다.
	TArray<FOverlapResult> OverlapResults;

	//CollisionQueryParam(tag, bTraceComplex, ignorePawn)
	//tag:충돌을 추적할 오브젝트들의 태그. NAME_None으로 지정하면 따로 태그를 따지지 않음
	//bTraceComplex:좀더 복잡한 충돌 처리 여부
	//ignorePawn:충돌 추적에서 제외할 폰. 여기서는 현재 이 폰을 무시했다.
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult) {
		//범위 기반 for문으로 OverlapResults순회
		//범위 기반 for문의 매개변수를 주소(&)로 선언하게될경우 매개변수에 순회하게될 Container의 원소들을 담지 않고, 그 원소를 참조한 참조값만 가지고 오게된다.
		//이렇게되면 기존방식으로 선언할 때 보다 복사를 하지 않게되니 포퍼먼스가 올라가며, 실제 원소를 참조하므로 값 또한 변경할 수 있다는 장점이 있다.
		//허나 밑의 예제에선 const를 붙여 선언하여 값을 변경하는 것을 막았다.
		for (auto const& OverlapResult : OverlapResults) {
			AABCharacter* ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor());
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController()) {
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

				DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}

