// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat) {
	ABCHECK(nullptr != NewCharacterStat);
	CurrentCharacterStat = NewCharacterStat;
	//AddUObject메서드는 대리자에 람다가 아닌 멤버함수를 바인딩할때 사용합니다.
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

void UABCharacterWidget::NativeConstruct() {
	Super::NativeConstruct();
	//GetWidgetFromName메서드는 위젯 블루 프린트의 요소를 이름을 통해 가져올 수 있다.
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget() {
	if (CurrentCharacterStat.IsValid()) {
		if (nullptr != HPProgressBar) {
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}

