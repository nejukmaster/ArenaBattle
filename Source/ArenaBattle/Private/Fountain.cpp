
// Fill out your copyright notice in the Description page of Project Settings.


#include "Fountain.h"

// Sets default values
AFountain::AFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CreateDefaultSubobject는 실재 컴포넌트를 생성합니다. 인자로 받는 문자열은 액터를 상속받은 컴포넌트 각각을 구분하기위한 Hash를 생성하는 key로 사용됩니다. 따라서 중복되면 안됩니다.
	//TEXT매크로는 해당 문자를 플랫폼이 바뀌어도 유지되는 형식의 2바이트 문자열로 바꾸어줍니다.
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));
	
	//RootComponent는 이 액터의 맵상에서의 트랜스폼과 충돌을 검사할 컴포넌트로, 스태틱/ 스켈레톤 매쉬여야 합니다.
	RootComponent = Body;
	//Water매쉬를 Body매쉬의 자식오브젝트로 설정
	Water->SetupAttachment(Body);
	Light->SetupAttachment(Body);
	Splash->SetupAttachment(Body);
	//SetRelativeLocation은 컴포넌트의 기본 위치값을 변경해준다.
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));
	Splash->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	//ConstructorHelpers의 FObjectFinder클래스를 사용하여 FObjectFinder변수 SM_BODY를 선언한다.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	//성공적으로 에셋을 불러왔을경우
	if (SM_BODY.Succeeded()) {
		//스태틱 매쉬 Body를 불러온 에셋의 스태틱매쉬로 설정합니다.
		Body->SetStaticMesh(SM_BODY.Object);
	}
	else ABLOG(Warning, TEXT("Body Loaded fail!"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WATER(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02"));
	if (SM_WATER.Succeeded()) {
		Water->SetStaticMesh(SM_WATER.Object);
	}
	else ABLOG(Warning, TEXT("Water Loaded fail!"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_SPLASH(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01"));
	if (PS_SPLASH.Succeeded()) {
		Splash->SetTemplate(PS_SPLASH.Object);
	}
	else ABLOG(Warning, TEXT("FX Loaded fail!"));

	RotateSpeed = 30.0f;
	//c++에서 ->는 다른언의 .(dot)과 비슷한 역할을 하며 얕은참조를 한 포인터의 데이터의 멤버에 접근할때 사용한다.
	//c++에서 .(dot)은 깊은참조를 하여 값을 복사해온 데이터의 멤버에 접근할때 사용한다.
	Movement->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f);
}

// Called when the game starts or whven spawned
void AFountain::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG매크로는 실제 로그를 출력하는 매크로이다.
	//GetName은 현재 액터의 이름을 반환하는 함수이며 FString 타입으로 반환을 하나, FString타입을 문자열로 가져올 경우 반드시 *(참조 연산자)를 사용하도록 한다.
	//UE_LOG(ArenaBattle, Log, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);

	//매크로 사용으로 로그 출력
	ABLOG_S(Warning);
	ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);
}

void AFountain::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	ABLOG_S(Warning);
}

void AFountain::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

// Called every frame
void AFountain::Tick(float DeltaTime)
{
	/*
	Super::Tick(DeltaTime);
	//AddActorLocalRotation은 이 엑터의 회전에 로테이션을 추가합니다.
	//FRotator(Pitch,Yaw,Roll)은 언리얼의 회적각을 반환합니다.
	//DeltaTime은 프레임간의 시간을 반영하며, GetWorld()->GetDeltaSeconds()로 Tick함수 외부에서도 얻을수 있다.
	AddActorLocalRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f));
	*/
}

