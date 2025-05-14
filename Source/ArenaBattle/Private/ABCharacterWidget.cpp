// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat) {
	ABCHECK(nullptr != NewCharacterStat);
	CurrentCharacterStat = NewCharacterStat;
	//AddUObject�޼���� �븮�ڿ� ���ٰ� �ƴ� ����Լ��� ���ε��Ҷ� ����մϴ�.
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

void UABCharacterWidget::NativeConstruct() {
	Super::NativeConstruct();
	//GetWidgetFromName�޼���� ���� ��� ����Ʈ�� ��Ҹ� �̸��� ���� ������ �� �ִ�.
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

