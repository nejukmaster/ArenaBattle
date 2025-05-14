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

	//autoŰ����� ���� �ʱ�ȭ �Ŀ��� ������ �߷��Ѵ�.
	//TryGetPawnOwner�� ��ȿ�� Pawn������Ʈ�� �����͸� ��ȯ�Ѵ�.
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	//ĳ���Ͱ� ���� �ʾ��� ��쿡�� ����
	if (!IsDead) {
		//���� ���� ������ ::�տ� �ƹ��͵� ������ ������� ��������/�Լ��� �����Ѵ�.
		if (::IsValid(Pawn)) {
			CurrentPawnSpeed = Pawn->GetVelocity().Size();
			auto Character = Cast<ACharacter>(Pawn);
			if (Character) {
				//CharacterMovement�� IsFalling�� ���߿� ���ִ��� �ƴ����� ��ȯ�Ѵ�.
				IsInAir = Character->GetMovementComponent()->IsFalling();
			}
		}
	}
}

void UABAnimInstance::PlayAttackMontage() {
	ABCHECK(!IsDead);
	//Montage_IsPlaying(UAnimMontage)�� ���ڷ� �� ��Ÿ�ְ� ����������� ��ȯ�Ѵ�.
	//if (!Montage_IsPlaying(AttackMontage)) 
	//Montage_Play�� ������ ��Ÿ�ָ� �־��� ������� ����մϴ�.
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection) {
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection),AttackMontage);
}

void UABAnimInstance::AnimNotify_AttackHitCheck() {
	//��������Ʈ�� Broadcast �޼���� �ش� ��������Ʈ�� ��ϵ� ��� �Լ��� ȣ���մϴ�.
	OnAttackHitCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_NextAttackCheck() {
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section) {
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
