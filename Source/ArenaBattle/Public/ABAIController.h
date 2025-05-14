// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();
	//4.22버전 이후 부터의 언리얼에선 AController::Possess함수를 재정의 불가능 하게 final로 선언되었기에 OnPossess함수를 대신 사용한다.
	virtual void OnPossess(APawn* InPawn) override;
	//4.22버전 이후 부터의 언리얼에선 AController::UnPossess함수를 재정의 불가능 하게 final로 선언되었기에 OnUnPossess함수를 대신 사용한다.
	//virtual void OnUnPossess() override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
};
