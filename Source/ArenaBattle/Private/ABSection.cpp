// Fill out your copyright notice in the Description page of Project Settings.

#include "ABSection.h"
#include "ABPlayerController.h"
#include "ABCharacter.h"
#include "ABItemBox.h"
#include "ABGameMode.h"

// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	FString AssetPath = TEXT("/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded()) {
		Mesh->SetStaticMesh(SM_SQUARE.Object);
	}
	else {
		ABLOG(Error, TEXT("Failed  to load staticmesh asset. : &s"), *AssetPath);
	}

	//스테이지 매쉬의 콜리전 트리거 설정
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

	FString GateAssetPath = TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);
	if (!SM_GATE.Succeeded()) {
		ABLOG(Error, TEXT("Failed to load staticmesh asset. : %s"), *GateAssetPath);
	}

	static FName GateSockets[] = { {TEXT("+XGate")},{TEXT("-XGate")},{TEXT("+YGate")},{TEXT("-YGate")} };
	for (FName GateSocket : GateSockets) {
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
		GateMeshes.Add(NewGate);

		//스테이지에 배치된 게이트의 콜리전 트리거 설정
		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);
	}

	bNoBattle = false;

	EnemySpawnTime = 2.0f;
	ItemBoxSpawnTime = 5.0f;
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	ABLOG(Warning,TEXT("BeginPlay"))
	//UBoxComponent의 대리자에 다이나믹으로 함수를 추가하는 작업은 AddDynamic메서드를 통해 이뤄지며, 이는 생성자에서는 동작하지 않으므로 BeginPlay에서 구현하도록 한다.
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);
	static FName GateSockets[] = { {TEXT("+XGate")},{TEXT("-XGate")},{TEXT("+YGate")},{TEXT("-YGate")} };
	int i = 0;
	for (UBoxComponent* GateTrigger : GateTriggers) {
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		//UBoxComponent에 태그를 추가하는 작업 역시 생성자에선 동작하지 않으므로 BeginPlay에서 구현한다.
		GateTrigger->ComponentTags.Add(GateSockets[i]);
		i++;
	}
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::SetState(ESectionState NewState) {
	switch (NewState) {
	case ESectionState::READY: {
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers) {
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}

		OperateGates(true);
		break;
	}
	case ESectionState::BATTLE: {
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers) {
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}

		OperateGates(false);

		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);

		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
																														FVector2D RandXY = FMath::RandPointInCircle(600.0f);
																														GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);
			}), ItemBoxSpawnTime, false);
		break;
	}
	case ESectionState::COMPLETE: {
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers) {
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		}

		OperateGates(true);
		break;
	}
	}

	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen) {
	for (UStaticMeshComponent* Gate : GateMeshes) {
		Gate->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
}

void AABSection::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSeep, const FHitResult& SweepResult) {
	ABLOG(Warning, TEXT("test"));
	if (CurrentState == ESectionState::READY) {
		SetState(ESectionState::BATTLE);
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSeep, const FHitResult& SweepResult) {
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	//문자열의 Left(int count)메서드는 문자열의 왼쪽에서부터 count개의 문자를 추출한다.
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (!Mesh->DoesSocketExist(SocketName))
		return;
	FVector NewLocation = Mesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		ObjectQueryParam,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

	if (!bResult) {
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else {
		ABLOG(Warning, TEXT("New section area is not empty."));
	}
}

void AABSection::OnNPCSpawn() {
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
	auto KeyNPC = GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector + 88.0f, FRotator::ZeroRotator);
	if (nullptr != KeyNPC) {
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestoryed);
	}
}

void AABSection::OnKeyNPCDestoryed(AActor* DestoryedActor) {
	auto ABCharacter = Cast<AABCharacter>(DestoryedActor);
	ABCHECK(nullptr != ABCharacter);

	//LashHitBy속성에는 데미지 프레임워크상에서 이 엑터에 마지막으로 피해를 입힌 AController가 저장되어있다.
	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
	ABCHECK(nullptr != ABPlayerController);

	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABCHECK(nullptr != ABGameMode);
	ABGameMode->AddScore(ABPlayerController);

	SetState(ESectionState::COMPLETE);
}

// Called every frame
void AABSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

