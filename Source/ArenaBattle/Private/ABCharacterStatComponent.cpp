// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	
	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UABCharacterStatComponent::InitializeComponent() {
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel) {
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != ABGameInstance);
	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel);
	if (nullptr != CurrentStatData) {
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
		CurrentHP = CurrentStatData->MaxHP;
	}
	else {
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UABCharacterStatComponent::SetDamage(float NewDamage) {
	ABCHECK(nullptr != CurrentStatData);
	//FMath클래스의 Clamp(value,min,max) 정적 메서드는 value를 min과 max값으로 클램핑한 값을 반환한다.
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
}

void UABCharacterStatComponent::SetHP(float NewHP) {
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	//KINDA_SAMLL_NUMBER은 실수값 0의 아주 미세한 오차만큼 큰값으로 실수로 계산할때 0을 사용할경우 이 오차를 놓칠 수 있기때문에 이 매크로를 사용한다.
	if (CurrentHP < KINDA_SMALL_NUMBER) {
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

int32 UABCharacterStatComponent::GetDropExp() const {
	return CurrentStatData->DropExp;
}

float UABCharacterStatComponent::GetAttack() {
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}

float UABCharacterStatComponent::GetHPRatio() {
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	//MaxHP가 0이 아닐경우에 비율로 출력, 그렇지 않을 경우 0으로 출력
	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

