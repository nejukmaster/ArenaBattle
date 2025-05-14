// Fill out your copyright notice in the Description page of Project Settings.

//GameMode는 언리얼에서 제공하는 프레임워크중 하나로 게임의 규칙을 정의한다.
#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"
#include "EngineUtils.h"


AABGameMode::AABGameMode() {
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();
	ScoreToClear = 2;
}

void AABGameMode::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

void AABGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData();
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer) {
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController)) {
			ABPlayerController->AddGameScore();
			break;
		}
	}
	ABGameState->AddGameScore();

	if (GetScore() >= ScoreToClear) {
		ABGameState->SetGameCleared();
		//책에서는 GetWord()->GetPawnIterator()를 사용하나, UE5에선 이 기능이 Deprecated되었으므로, 대신 TActorIterator를 사용하여 월드의 폰을 순회한다.
		//이떄 TActorIterator를 사용하기위해선 "EngineUtils.h"헤더파일을 인클루드해야한다.
		for (TActorIterator<APawn> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			(*ActorItr)->TurnOff();
		}
		
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
			if (nullptr != ABPlayerController) {
				ABPlayerController->ShowResultUI();
			}
		}
	}
}

int32 AABGameMode::GetScore() const {
	return ABGameState->GetTotalGameScore();
}