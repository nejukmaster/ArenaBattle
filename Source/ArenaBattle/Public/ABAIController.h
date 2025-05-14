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
	//4.22���� ���� ������ �𸮾󿡼� AController::Possess�Լ��� ������ �Ұ��� �ϰ� final�� ����Ǿ��⿡ OnPossess�Լ��� ��� ����Ѵ�.
	virtual void OnPossess(APawn* InPawn) override;
	//4.22���� ���� ������ �𸮾󿡼� AController::UnPossess�Լ��� ������ �Ұ��� �ϰ� final�� ����Ǿ��⿡ OnUnPossess�Լ��� ��� ����Ѵ�.
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
