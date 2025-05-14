
// Fill out your copyright notice in the Description page of Project Settings.


#include "Fountain.h"

// Sets default values
AFountain::AFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CreateDefaultSubobject�� ���� ������Ʈ�� �����մϴ�. ���ڷ� �޴� ���ڿ��� ���͸� ��ӹ��� ������Ʈ ������ �����ϱ����� Hash�� �����ϴ� key�� ���˴ϴ�. ���� �ߺ��Ǹ� �ȵ˴ϴ�.
	//TEXT��ũ�δ� �ش� ���ڸ� �÷����� �ٲ� �����Ǵ� ������ 2����Ʈ ���ڿ��� �ٲپ��ݴϴ�.
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));
	
	//RootComponent�� �� ������ �ʻ󿡼��� Ʈ�������� �浹�� �˻��� ������Ʈ��, ����ƽ/ ���̷��� �Ž����� �մϴ�.
	RootComponent = Body;
	//Water�Ž��� Body�Ž��� �ڽĿ�����Ʈ�� ����
	Water->SetupAttachment(Body);
	Light->SetupAttachment(Body);
	Splash->SetupAttachment(Body);
	//SetRelativeLocation�� ������Ʈ�� �⺻ ��ġ���� �������ش�.
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));
	Splash->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	//ConstructorHelpers�� FObjectFinderŬ������ ����Ͽ� FObjectFinder���� SM_BODY�� �����Ѵ�.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	//���������� ������ �ҷ��������
	if (SM_BODY.Succeeded()) {
		//����ƽ �Ž� Body�� �ҷ��� ������ ����ƽ�Ž��� �����մϴ�.
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
	//c++���� ->�� �ٸ����� .(dot)�� ����� ������ �ϸ� ���������� �� �������� �������� ����� �����Ҷ� ����Ѵ�.
	//c++���� .(dot)�� ���������� �Ͽ� ���� �����ؿ� �������� ����� �����Ҷ� ����Ѵ�.
	Movement->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f);
}

// Called when the game starts or whven spawned
void AFountain::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG��ũ�δ� ���� �α׸� ����ϴ� ��ũ���̴�.
	//GetName�� ���� ������ �̸��� ��ȯ�ϴ� �Լ��̸� FString Ÿ������ ��ȯ�� �ϳ�, FStringŸ���� ���ڿ��� ������ ��� �ݵ�� *(���� ������)�� ����ϵ��� �Ѵ�.
	//UE_LOG(ArenaBattle, Log, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"), *GetName(), ID, GetActorLocation().X);

	//��ũ�� ������� �α� ���
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
	//AddActorLocalRotation�� �� ������ ȸ���� �����̼��� �߰��մϴ�.
	//FRotator(Pitch,Yaw,Roll)�� �𸮾��� ȸ������ ��ȯ�մϴ�.
	//DeltaTime�� �����Ӱ��� �ð��� �ݿ��ϸ�, GetWorld()->GetDeltaSeconds()�� Tick�Լ� �ܺο����� ������ �ִ�.
	AddActorLocalRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f));
	*/
}

