// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"

AABPlayerState::AABPlayerState() {
	CharacterLevel = 1;
	GameScore = 0;
	GameHighScore = 0;
	Exp = 0;
	SaveSlotName = TEXT("Player1");
	CharacterIndex = 0;
}

int32 AABPlayerState::GetGameScore() const {
	return GameScore;
}

int32 AABPlayerState::GetGameHightScore() const {
	return GameHighScore;
}

int32 AABPlayerState::GetCharacterLevel() const {
	return CharacterLevel;
}

int32 AABPlayerState::GetCharacterIndex() const {
	return CharacterIndex;
}

void AABPlayerState::InitPlayerData() {
	//LoadGameFromSlot은 sav파일을 SaveGame형태로 불러온다.
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (nullptr == ABSaveGame) {
		//GetMutableDefault는 해당 클래스의 기본 객체를 들고온다.
		ABSaveGame = GetMutableDefault<UABSaveGame>();
	}

	SetPlayerName(ABSaveGame->PlayerName);
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	CharacterIndex = ABSaveGame->CharacterIndex;
	SavePlayerData();
}

void AABPlayerState::SavePlayerData() {
	//언리얼에서 새 오브젝트를 생성할때는 NewObject메서드를 사용한다. 이렇게 생성한 언리얼 오브젝트는 언리얼 엔진의 가비지 컬렉터를 통해 관리된다.
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;
	NewPlayerData->CharacterIndex = CharacterIndex;

	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0)) {
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}

float AABPlayerState::GetExpRatio() const {
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.0f;
	
	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp) {
	if (CurrentStatData->NextExp == -1)
		return false;
	
	bool DidLevelUp = false;
	Exp = Exp + IncomeExp;
	if (Exp >= CurrentStatData->NextExp) {
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}


	OnPlayerStateChangedDelegate.Broadcast();
	SavePlayerData();
	return DidLevelUp;
}

void AABPlayerState::AddGameScore() {
	GameScore++;
	if (GameScore >= GameHighScore) {
		GameHighScore = GameScore;
	}
	OnPlayerStateChangedDelegate.Broadcast();
	SavePlayerData();
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel) {
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}