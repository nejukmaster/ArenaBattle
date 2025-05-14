// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InputActionValue.h"
#include "ABPawn.generated.h"

UCLASS()
class ARENABATTLE_API AABPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AABPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//충돌을 담당할 캡슐 컴포넌트입니다.
	//또한 이 폰을 대표해 게임 세계에서의 움직임을 담당합니다.
	UPROPERTY(VisibleAnywhere, Category = Collision)
	UCapsuleComponent* Capsule;

	//캐릭터의 에셋을 보여주고 애니메이션도 담당합니다.
	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* Mesh;

	//플레이어의 입력을 받아 캐릭터의 움직임을 설정하는 컴포넌트 입니다.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UFloatingPawnMovement* Movement;

	//삼인칭 시점으로 카메라의 구도를 설정하게 해주는 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	//폰에 카메라컴포넌트를 부착하면 폰이 빙의된 캐릭터 플레이어에게 이 카메라의 화면을 전송합니다.
	//스프링암 컴포넌트의 자식객체로 설정하면 스프링암에 의해 3인칭 카메라 무브로 움직입니다.
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

private:
	void Move(const FInputActionValue& NewAxisValue);

	//언리얼 엔진의 Enhanced Input으로 설정한 매핑 컨텍스트
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	//사용할 Enhanced Input Action
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* MoveAction;

};
