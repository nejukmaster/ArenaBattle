// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "ABGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase {

	GENERATED_BODY()

public:
	/*기본 컨스트럭터에 각 요소를 초기화 한다.
	*
	* public Constructor(){
	*	Level = 1;
	*	MaxHP = 100.0f;
	*	Attack = 10.0f;
	*	DropExp = 10;
	*	NextExp = 30;
	* }
	* 
	* 과 동치
	*/
	FABCharacterData() : Level(1), MaxHP(100.0f), Attack(10.0f), DropExp(10), NextExp(30) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 NextExp;
};
/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UABGameInstance();

	virtual void Init() override;
	FABCharacterData* GetABCharacterData(int32 Level);

	//FStreamableManager클래스는 언리얼에서 비동기 에셋 로딩을 하는데 사용하는 클래스이다.
	//비동기 에셋 로딩은 런타임에서 에셋을 로드/언로드하는 행위를 말한다.
	FStreamableManager StreamableManager;
	
private:
	UPROPERTY()
	class UDataTable* ABCharacterTable;
};
