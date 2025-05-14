// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

//캐릭터의 현재 상태를 나타내줄 스테이트머신용 Enum클래스
//UENUM(BlueprintType)으로 설정하면 해당 Enum클래스를 블루프린트에서도 사용할 수 있다.
UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	PREINIT,
	LOADING,
	READY,
	DEAD
};

//DECLARE_LOG_CATEGORY_EXTERN(CategoryName, Default Verbose, Compile Time Verbose)매크로는 해당 카테고리의 로그가 얼마나 상세한 정보를 표기할지를 정의한다.
DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);
//로그 출력을 간단하게 할 매크로 선언
//__FUNCTION__은 이 매크로를 호출한 함수 이름을, __LINE__은 이 코드를 호출한 소스의 몇번째줄인지를 출력합니다.(__LINE__은 int형이므로 FromInt를 통해 문자열로 변환합니다.)
#define ABLOG_CALLINFO (FString(__FUNCTION__)+TEXT("(")+FString::FromInt(__LINE__)+TEXT(")"))
#define ABLOG_S(Verbosity) UE_LOG(ArenaBattle, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
#define ABLOG(Verbosity, Format,...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s %s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define ABCHECK(Expr, ...){if(!(Expr))	{ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;	}}
