// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance() {
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded()) {
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	//auto키워드는 변수 초기화 식에서 형식을 추론한다.
	//TryGetPawnOwner는 유효한 Pawn오브젝트의 포인터를 반환한다.
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	//캐릭터가 죽지 않았을 경우에만 실행
	if (!IsDead) {
		//범위 지정 연산자 ::앞에 아무것도 붙이지 않을경우 전역변수/함수에 접근한다.
		if (::IsValid(Pawn)) {
			CurrentPawnSpeed = Pawn->GetVelocity().Size();
			auto Character = Cast<ACharacter>(Pawn);
			if (Character) {
				//CharacterMovement의 IsFalling은 공중에 떠있는지 아닌지를 반환한다.
				IsInAir = Character->GetMovementComponent()->IsFalling();
			}
		}
	}
}

void UABAnimInstance::PlayAttackMontage() {
	ABCHECK(!IsDead);
	//Montage_IsPlaying(UAnimMontage)는 인자로 준 몽타주가 재생중인지를 반환한다.
	//if (!Montage_IsPlaying(AttackMontage)) 
	//Montage_Play는 인자의 몽타주를 주어진 배속으로 재생합니다.
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection) {
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection),AttackMontage);
}

void UABAnimInstance::AnimNotify_AttackHitCheck() {
	//델리게이트의 Broadcast 메서드는 해당 델리게이트에 등록된 모든 함수를 호출합니다.
	OnAttackHitCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_NextAttackCheck() {
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section) {
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
