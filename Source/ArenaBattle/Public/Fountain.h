// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
//CoreMinimal ����Ʈ ����� �� ������Ʈ�� �𸮾�� �����ϱ� ���� �ּ����� ����� �츮�� c++�� ����� ����ϱ� ���� EngineMinimal�� �ٲپ��ݴϴ�.
#include "EngineMinimal.h"
//�𸮾� ������ �����ϴ� �����Ʈ ������Ʈ�� ������ �����ӿ� ���� ���� ����� �����Ѵ�.
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
//generated.h�� �𸮾��� �� �ҽ��� �������ϱ� ���� �����ϴ� �𸮾� ȯ�濡 ���� ������ �����ϴ� �����, ��Ŭ��� �������� �ݵ�� �������ݴϴ�.
#include "Fountain.generated.h"

//�𸮾� Ŭ������ �� Ŭ������ �����ϴ� ��ũ���Դϴ�.
UCLASS()
//(����)_API Ű����� Ÿ ��⿡�� �� Ŭ������ ������ �� �ִ����� ���� �����Դϴ�.
class ARENABATTLE_API AFountain : public AActor
{
	//�𸮾� Ŭ������ �ݵ�� GENERATED_BODY��ũ�θ� �������ݴϴ�.
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//�����÷��̰� ����Ǿ����� ȣ��˴ϴ�.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//BeginPlayȣ�� ������ ȣ��˴ϴ�.
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY�� �𸮾�Ŭ���� Ÿ���� ������ ������ ���� ��밡���� ��ũ�η� �� ������ ������ �ʰԵǸ� �ڵ����� �Ҵ��� �������ݴϴ�.
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Body;


	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Water;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent *Light;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *Splash;

	//int32�� �𸮾� ������ �����ϴ� 32��Ʈ ¥�� ���� �ڷ����̴�.
	UPROPERTY(EditAnywhere, Category=ID)
	int32 ID;

	//URotatingMovementComponent�� 
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

private:
	//Meta Ű����� �� UPROPERTY�� ��Ÿ�����͸� ������ �� �ְ� ���ָ�, AllowPrivateAccess�� private������ �ν����Ϳ��� ������ �� �ְ� ���ش�.
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;

};
