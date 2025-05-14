// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

//ĳ������ ���� ���¸� ��Ÿ���� ������Ʈ�ӽſ� EnumŬ����
//UENUM(BlueprintType)���� �����ϸ� �ش� EnumŬ������ �������Ʈ������ ����� �� �ִ�.
UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	PREINIT,
	LOADING,
	READY,
	DEAD
};

//DECLARE_LOG_CATEGORY_EXTERN(CategoryName, Default Verbose, Compile Time Verbose)��ũ�δ� �ش� ī�װ��� �αװ� �󸶳� ���� ������ ǥ�������� �����Ѵ�.
DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);
//�α� ����� �����ϰ� �� ��ũ�� ����
//__FUNCTION__�� �� ��ũ�θ� ȣ���� �Լ� �̸���, __LINE__�� �� �ڵ带 ȣ���� �ҽ��� ���°�������� ����մϴ�.(__LINE__�� int���̹Ƿ� FromInt�� ���� ���ڿ��� ��ȯ�մϴ�.)
#define ABLOG_CALLINFO (FString(__FUNCTION__)+TEXT("(")+FString::FromInt(__LINE__)+TEXT(")"))
#define ABLOG_S(Verbosity) UE_LOG(ArenaBattle, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
#define ABLOG(Verbosity, Format,...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s %s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define ABCHECK(Expr, ...){if(!(Expr))	{ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;	}}
