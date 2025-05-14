// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

//DECLARE_MULTICAST_DELEGATE매크로는 여러개의 함수를 등록할 수 있는 커스텀 멀티캐스트 델리게이트를 선언해 줍니다.
//이 델리게이트는 NextAttackCheck노티파이가 발생할때마다 ABCharacter에 이를 전달하는 역할을 한다.
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
	//컨스트럭터
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck; 
	void SetDeadAnim() { IsDead = true; };

private:
	//BlueprintReadOnly는 블루프린트에서 접근하는 방법에대한 키워드이며, BlueprintReadOnly와 BlueprintReadWrite가 있다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	

	//몽타주의 애님노티파이가 실행할 메서드 설정
	//언리얼에선 AnimNotify_(노티파이 명)으로 메서드를 애님 인스턴스 내에서 선언할 경우 해당 노티하이가 호출되면 자동으로 이 메서드를 찾아서 실행한다.
	//이때 메서드는 언리얼 런타임에서 찾을수 있도록 반드시 UFUNCTION으로 지정되어야 한다.
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
