// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

//DECLARE_MULTICAST_DELEGATE��ũ�δ� �������� �Լ��� ����� �� �ִ� Ŀ���� ��Ƽĳ��Ʈ ��������Ʈ�� ������ �ݴϴ�.
//�� ��������Ʈ�� NextAttackCheck��Ƽ���̰� �߻��Ҷ����� ABCharacter�� �̸� �����ϴ� ������ �Ѵ�.
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	//����Ʈ����
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck; 
	void SetDeadAnim() { IsDead = true; };

private:
	//BlueprintReadOnly�� �������Ʈ���� �����ϴ� ��������� Ű�����̸�, BlueprintReadOnly�� BlueprintReadWrite�� �ִ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	

	//��Ÿ���� �ִԳ�Ƽ���̰� ������ �޼��� ����
	//�𸮾󿡼� AnimNotify_(��Ƽ���� ��)���� �޼��带 �ִ� �ν��Ͻ� ������ ������ ��� �ش� ��Ƽ���̰� ȣ��Ǹ� �ڵ����� �� �޼��带 ã�Ƽ� �����Ѵ�.
	//�̶� �޼���� �𸮾� ��Ÿ�ӿ��� ã���� �ֵ��� �ݵ�� UFUNCTION���� �����Ǿ�� �Ѵ�.
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsDead;
};
