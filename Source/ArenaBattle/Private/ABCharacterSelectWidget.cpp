// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterSelectWidget.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "ABSaveGame.h"
#include "ABPlayerState.h"

void UABCharacterSelectWidget::NextCharacter(bool bForward) {
	bForward ? CurrentIndex++ : CurrentIndex--;

	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;

	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];

	auto ABGameInstance = GetWorld()->GetGameInstance < UABGameInstance>();
	ABCHECK(nullptr != ABGameInstance);
	ABCHECK(TargetComponent.IsValid());

	//런타임에서 에셋을 로드
	USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
	if (nullptr != Asset) {
		TargetComponent->SetSkeletalMesh(Asset);
	}
}

void UABCharacterSelectWidget::NativeConstruct() {
	Super::NativeConstruct();

	CurrentIndex = 0;
	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	MaxIndex = CharacterSetting->CharacterAssets.Num();

	//TActorIterator<class>는 생성자의 인자로 주어진 월드의 class를 상속받은 액터들의 이터레이터를 들고온다.
	for(TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It) {
		TargetComponent = It->GetSkeletalMeshComponent();
		break;
	}

	PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
	ABCHECK(nullptr != PrevButton);

	NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
	ABCHECK(nullptr != NextButton);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
	ABCHECK(nullptr != TextBox);

	ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
	ABCHECK(nullptr != ConfirmButton);

	PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked);
	NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClicked);
}

void UABCharacterSelectWidget::OnPrevClicked() {
	NextCharacter(false);
}

void UABCharacterSelectWidget::OnNextClicked() {
	NextCharacter(true);
}

void UABCharacterSelectWidget::OnConfirmClicked() {
	FString CharacterName = TextBox->GetText().ToString();
	if (CharacterName.Len() <= 0 || CharacterName.Len() > 10) return;

	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = CharacterName;
	NewPlayerData->Level = 1;
	NewPlayerData->Exp = 0;
	NewPlayerData->HighScore = 0;
	NewPlayerData->CharacterIndex = CurrentIndex;

	auto ABPlayerState = GetDefault<AABPlayerState>();
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0)) {
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Gameplay"));
	}
	else {
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}
