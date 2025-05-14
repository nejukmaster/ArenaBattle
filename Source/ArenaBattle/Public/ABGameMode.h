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
	//�÷��̾ �α���(������ ����)�ϰԵ� ��� �α����� �Ϸ��� ���Ŀ� PostLogin�Լ��� ȣ��Ǵµ� �� �Լ� ���ο��� �÷��̾ ������ ���� �����ϰ�, �÷��̾� ��Ʈ�ѷ��� �ش� ���� ����(possess)�ϴ� �۾��� �̷������.
	//�� ���� ��Ʈ�ѷ��� �����Ǵ� �������� PostInitializeComponents�Լ��� ȣ��ǰ�, ���ǵǴ� ���� �÷��̾� ��Ʈ�ѷ� Ŭ������ Possess�Լ�����, �����ϴ� ��Ʈ�ѷ��� ���� PossessedBy�Լ��� ������ �ִ�.
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AddScore(class AABPlayerController* ScoredPlayer);
	int32 GetScore() const;

private:
	UPROPERTY()
	class AABGameState* ABGameState;

	UPROPERTY()
	int32 ScoreToClear;
};
