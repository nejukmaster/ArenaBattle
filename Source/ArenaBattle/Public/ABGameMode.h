// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AABGameMode();
	
	virtual void PostInitializeComponents() override;
	//플레이어가 로그인(게임을 실행)하게될 경우 로그인을 완료한 이후에 PostLogin함수가 호출되는데 이 함수 내부에서 플레이어가 조종할 폰을 생성하고, 플레이어 컨트롤러가 해당 폰에 빙의(possess)하는 작업이 이루어진다.
	//각 폰과 컨트롤러가 생성되는 시점에는 PostInitializeComponents함수가 호출되고, 빙의되는 폰은 플레이어 컨트롤러 클래스의 Possess함수에서, 빙의하는 컨트롤러는 폰의 PossessedBy함수로 얻을수 있다.
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AddScore(class AABPlayerController* ScoredPlayer);
	int32 GetScore() const;

private:
	UPROPERTY()
	class AABGameState* ABGameState;

	UPROPERTY()
	int32 ScoreToClear;
};
