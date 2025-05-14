// Fill out your copyright notice in the Description page of Project Settings.

//Pawn은 언리얼에서 제공하는 프레임워크중 하나로, 게임모드 상에서 플레이어가 움직일 액터를 정의한다.
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

	//캡슐은 이 폰을 대표해 움직임을 담당하므로 루트 컴포넌트로 넣습니다. 이래야 캡슐이 움직이며 변한 트랜스폼이 다른 컴포넌트에도 적용이 되기 때문입니다.
	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	//캡슐의 크기를 사용할 캐릭터 에셋에 맞게 설정합니다.
	Capsule->SetCapsuleHalfHeight(88.0f);
	Capsule->SetCapsuleRadius(34.0f);
	//사용하려는 에셋의 원점(origin)이 발바닥에 있고, 언리얼에서 기준점은 엑터의 정중앙을 잡으므로 상대위치를 절반만큼 z축으로 내려주어 이 두점을 맞춰줍니다.
	//또한 에셋의 좌표계와 언리얼의 좌표계가 서로 다른경우도 있으므로 이를 확인하여 맞춰주도록합니다.
	Mesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	//카메라의 지지대 길이를 설정합니다(카메라로부터 플레이어까지의 거리)
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded()) {
		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	//매핑 컨텍스트 에셋을 가져옵니다.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT(TEXT("/Game/ABInputMappingCtx.ABInputMappingCtx"));
	if (DEFAULT_CONTEXT.Succeeded())
		DefaultContext = DEFAULT_CONTEXT.Object;

	//사용할 인풋 액션 에셋을 가져옵니다.
	static ConstructorHelpers::FObjectFinder<UInputAction> AB_MOVE(TEXT("/Game/ABMove.ABMove"));
	if (AB_MOVE.Succeeded())
		MoveAction = AB_MOVE.Object;
	//SetAnimationMode는 이 USkeletalMeshComponent의 애니메이션 모드를 설정합니다.
	//https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Components/EAnimationMode__Type/
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIME(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint"));
	if (WARRIOR_ANIME.Succeeded()) {
		//SetAnimInstanceClass는 이 USkeletalMeshComponent의 애니메이션 데이터를 생성할 클래스를 지정합니다.
		Mesh->SetAnimInstanceClass(WARRIOR_ANIME.Class);
	}

}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//GetController는 이 액터의 컨트롤러를 가져옵니다.
	//Cast 메서드는 인자를 주어진 클래스로 형변환 하고, 할수 없으면 false를 반환합니다.
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//서브 시스템과 매핑 컨텍스트를 연결하는 작업입니다.
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(DefaultContext, 0);
	}

	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//LoadObject는 게임 실행중에 에셋을 로드하는 명령어이다.
	UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Animations/WarriorRun.WarriorRun"));

	if (AnimAsset != nullptr) {
		//PlayAnimation(animAsset,isLooping)은 animAsset을 재생하고 isLooping이 true일경우 이를 루프한다.
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

	//Enhanced Input에서는 기본적인 PlayerInputComponent를 EnhancedInputComponent로 캐스팅해서 바인드 해주어야합니다.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
																						//함수의 주소값을 인자로 넘깁니다.
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