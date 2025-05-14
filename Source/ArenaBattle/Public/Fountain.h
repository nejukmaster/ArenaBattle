// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
//CoreMinimal 디폴트 헤더는 이 컴포넌트가 언리얼상에 동작하기 위한 최소한의 헤더로 우리는 c++의 기능을 사용하기 위해 EngineMinimal로 바꾸어줍니다.
#include "EngineMinimal.h"
//언리얼 엔진이 제공하는 무브먼트 컴포넌트는 액터의 움직임에 대한 여러 기능을 제공한다.
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
//generated.h는 언리얼이 이 소스를 컴파일하기 전에 생성하는 언리얼 환경에 대한 정보를 저장하는 헤더로, 인클루드 마지막에 반드시 참조해줍니다.
#include "Fountain.generated.h"

//언리얼 클래스로 이 클래스를 생성하는 매크로입니다.
UCLASS()
//(모듈명)_API 키워드는 타 모듈에서 이 클래스에 접근할 수 있는지에 대한 여부입니다.
class ARENABATTLE_API AFountain : public AActor
{
	//언리얼 클래스는 반드시 GENERATED_BODY매크로를 선언해줍니다.
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//게임플레이가 종료되었을때 호출됩니다.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//BeginPlay호출 직전에 호출됩니다.
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY는 언리얼클래스 타입의 변수를 선언할 때만 사용가능한 매크로로 이 변수가 사용되지 않게되면 자동으로 할당을 해제해줍니다.
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Body;


	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Water;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent *Light;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *Splash;

	//int32는 언리얼 엔진이 제공하는 32비트 짜리 정수 자료형이다.
	UPROPERTY(EditAnywhere, Category=ID)
	int32 ID;

	//URotatingMovementComponent는 
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

private:
	//Meta 키워드는 이 UPROPERTY의 메타데이터를 설정할 수 있게 해주며, AllowPrivateAccess는 private변수를 인스펙터에서 수정할 수 있게 해준다.
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;

};
