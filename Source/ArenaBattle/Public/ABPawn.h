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

	//�浹�� ����� ĸ�� ������Ʈ�Դϴ�.
	//���� �� ���� ��ǥ�� ���� ���迡���� �������� ����մϴ�.
	UPROPERTY(VisibleAnywhere, Category = Collision)
	UCapsuleComponent* Capsule;

	//ĳ������ ������ �����ְ� �ִϸ��̼ǵ� ����մϴ�.
	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* Mesh;

	//�÷��̾��� �Է��� �޾� ĳ������ �������� �����ϴ� ������Ʈ �Դϴ�.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UFloatingPawnMovement* Movement;

	//����Ī �������� ī�޶��� ������ �����ϰ� ���ִ� ������Ʈ�Դϴ�.
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	//���� ī�޶�������Ʈ�� �����ϸ� ���� ���ǵ� ĳ���� �÷��̾�� �� ī�޶��� ȭ���� �����մϴ�.
	//�������� ������Ʈ�� �ڽİ�ü�� �����ϸ� �������Ͽ� ���� 3��Ī ī�޶� ����� �����Դϴ�.
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

private:
	void Move(const FInputActionValue& NewAxisValue);

	//�𸮾� ������ Enhanced Input���� ������ ���� ���ؽ�Ʈ
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	//����� Enhanced Input Action
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* MoveAction;

};
