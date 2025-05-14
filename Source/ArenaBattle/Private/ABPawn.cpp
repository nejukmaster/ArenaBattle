// Fill out your copyright notice in the Description page of Project Settings.

//Pawn�� �𸮾󿡼� �����ϴ� �����ӿ�ũ�� �ϳ���, ���Ӹ�� �󿡼� �÷��̾ ������ ���͸� �����Ѵ�.
#include "ABPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
AABPawn::AABPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	//ĸ���� �� ���� ��ǥ�� �������� ����ϹǷ� ��Ʈ ������Ʈ�� �ֽ��ϴ�. �̷��� ĸ���� �����̸� ���� Ʈ�������� �ٸ� ������Ʈ���� ������ �Ǳ� �����Դϴ�.
	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	//ĸ���� ũ�⸦ ����� ĳ���� ���¿� �°� �����մϴ�.
	Capsule->SetCapsuleHalfHeight(88.0f);
	Capsule->SetCapsuleRadius(34.0f);
	//����Ϸ��� ������ ����(origin)�� �߹ٴڿ� �ְ�, �𸮾󿡼� �������� ������ ���߾��� �����Ƿ� �����ġ�� ���ݸ�ŭ z������ �����־� �� ������ �����ݴϴ�.
	//���� ������ ��ǥ��� �𸮾��� ��ǥ�谡 ���� �ٸ���쵵 �����Ƿ� �̸� Ȯ���Ͽ� �����ֵ����մϴ�.
	Mesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	//ī�޶��� ������ ���̸� �����մϴ�(ī�޶�κ��� �÷��̾������ �Ÿ�)
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded()) {
		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	//���� ���ؽ�Ʈ ������ �����ɴϴ�.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT(TEXT("/Game/ABInputMappingCtx.ABInputMappingCtx"));
	if (DEFAULT_CONTEXT.Succeeded())
		DefaultContext = DEFAULT_CONTEXT.Object;

	//����� ��ǲ �׼� ������ �����ɴϴ�.
	static ConstructorHelpers::FObjectFinder<UInputAction> AB_MOVE(TEXT("/Game/ABMove.ABMove"));
	if (AB_MOVE.Succeeded())
		MoveAction = AB_MOVE.Object;
	//SetAnimationMode�� �� USkeletalMeshComponent�� �ִϸ��̼� ��带 �����մϴ�.
	//https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Components/EAnimationMode__Type/
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIME(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint"));
	if (WARRIOR_ANIME.Succeeded()) {
		//SetAnimInstanceClass�� �� USkeletalMeshComponent�� �ִϸ��̼� �����͸� ������ Ŭ������ �����մϴ�.
		Mesh->SetAnimInstanceClass(WARRIOR_ANIME.Class);
	}

}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//GetController�� �� ������ ��Ʈ�ѷ��� �����ɴϴ�.
	//Cast �޼���� ���ڸ� �־��� Ŭ������ ����ȯ �ϰ�, �Ҽ� ������ false�� ��ȯ�մϴ�.
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//���� �ý��۰� ���� ���ؽ�Ʈ�� �����ϴ� �۾��Դϴ�.
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(DefaultContext, 0);
	}

	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//LoadObject�� ���� �����߿� ������ �ε��ϴ� ��ɾ��̴�.
	UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Animations/WarriorRun.WarriorRun"));

	if (AnimAsset != nullptr) {
		//PlayAnimation(animAsset,isLooping)�� animAsset�� ����ϰ� isLooping�� true�ϰ�� �̸� �����Ѵ�.
		Mesh->PlayAnimation(AnimAsset, true);
	}
}

// Called every frame
void AABPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Enhanced Input������ �⺻���� PlayerInputComponent�� EnhancedInputComponent�� ĳ�����ؼ� ���ε� ���־���մϴ�.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
																						//�Լ��� �ּҰ��� ���ڷ� �ѱ�ϴ�.
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABPawn::Move);
	}

}

void AABPawn::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPawn::PossessedBy(AController* NewController) {
	ABLOG_S(Warning);
	Super::PossessedBy(NewController);
}

void AABPawn::Move(const FInputActionValue& NewAxisValue) {
	FVector2D value = NewAxisValue.Get<FVector2D>();
	AddMovementInput(GetActorRightVector(), value.X);
	AddMovementInput(GetActorForwardVector(), value.Y);
}