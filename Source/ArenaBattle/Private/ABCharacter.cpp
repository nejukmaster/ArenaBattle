// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABCamera.h"
#include "ABAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
//언리얼엔진에서 제공하는 디버깅 툴인 디버그 드로잉 기능 사용을 위한 "DrawDebugHelpers.h"헤더파일 추가
#include "DrawDebugHelpers.h"
//DamageEvent가 선언된 헤더를 포함하여, 링크 오류 해결
#include "Engine/DamageEvents.h"
#include "ABCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include <iostream>
#include "ABCharacterSetting.h"
#include "ABWeapon.h"
#include "ABGameMode.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"

// Sets default values
AABCharacter::AABCharacter()

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	//TArray<AActor*> arrOutActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AABCamera::StaticClass(), arrOutActors);

	//Camera = ((AABCamera*)arrOutActors[0])->Camera;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	//ACharacter에셋에서는 ACharacer헤더에서 캡슐 컴포넌트를 자동으로 생성하기 때문에 따로 생성하는 과정이 필요없다.
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	//HP바 UI는 캐릭터를 따라다니며 머리 위쪽에 표시되어야되므로 메쉬의 하위 컴포넌트로 부착해준다.
	HPBarWidget->SetupAttachment(GetMesh());

	//ACharacter에셋은 USkeletalMeshComponent를 자동으로 생성하기 때문에 이를 따로 생성할 필요는 없다.
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	//USpringArmComponent역시 미리 정의되어있으므로 이를 사용한다.
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT(TEXT("/Game/ABInputMappingCtx.ABInputMappingCtx"));
	if (DEFAULT_CONTEXT.Succeeded())
		DefaultContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AB_MOVE(TEXT("/Game/ABMove.ABMove"));
	if (AB_MOVE.Succeeded())
		MoveAction = AB_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AB_LOOK(TEXT("/Game/ABLook.ABLook"));
	if (AB_LOOK.Succeeded())
		LookAction = AB_LOOK.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AB_VIEWCHANGE(TEXT("/Game/ABViewChange.ABViewChange"));
	if (AB_VIEWCHANGE.Succeeded())
		ViewChangeAction = AB_VIEWCHANGE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AB_ATTACK(TEXT("/Game/ABAttack.ABAttack"));
	if (AB_ATTACK.Succeeded())
		AttackAction = AB_ATTACK.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AB_JUMP(TEXT("/Game/ABJump.ABJump"));
	if (AB_JUMP.Succeeded())
		JumpAction = AB_JUMP.Object;

	FName WeaponSocket(TEXT("hand_rSocket"));
	//USkeletalMeshComponent의 DoesSocketExist메서드는 주어진 이름을 가진 소켓이 매쉬에 존재하는지 검사합니다.
	if (GetMesh()->DoesSocketExist(WeaponSocket)) {
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
		if (SK_WEAPON.Succeeded()) {
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}
		//SetupAttachment메서드는 해당 USkeletalMeshComponent를 주어진 소켓에 붙입니다.
		//인자로는 메쉬와 소켓의 이름을 받습니다.
		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	SetControllerMode(EControlMode::DIABLO);

	GetCharacterMovement()->JumpZVelocity = 800.0f;

	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	//콜리전 프리셋을 앞서 제작한 "ABCharacter"프리셋으로 변경해준다.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 80.0f;
	AttackRadius = 50.0f;

	//HP바의 위치를 캐릭터 머리위로 설정
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	//SetWidgetSpace메서드는 위젯을 어떤 형태로 보여줄지 정한다.
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded()) {
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	AssetIndex = 4;
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);

	DeadTimer = 5.0f;
}

void AABCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//애니메이션 인스턴스는 ConstructorHelpers를 통해 초기화 하지 않으므로 런타임 메서드인 PostInitializeComponents내에서 초기화 하여 사용합니다.
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	//델리게이트는 특정 객체가 해야할 로직을 다른 객체가 대신 처리해주는 로직을 말하며, 델리게이트에 함수를 등록하면 델리게이트에서 특정상황에 함수를 호출하는 형식으로 구현된다.
	//우리는 OnAttackMontageEnded를 등록하여 몽타주의 재생이 끝나면 델리게이트가 이를 통해 이 객체에 알려주는 형식으로 공격을 구현한다.
	//언리얼에서는 몽타주 재생이 끝나면 발동하는 OnMontageEnded라는 델리게이트를 제공한다.
	//AddDynamic은 델리게이트에 함수를 등록해준다. 등록해줄땐 this 키워드를 통해 등록한 오브젝트의 정보도 같이 넘겨주어야 한다.
	//또한 등록하는 함수는 반드시 UFUNCTION이어야만 한다.
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	//[](람다소개자):람다식에서 사용되는 []기호는 람다소개자로 해당 람다식이 실행될 환경을 지정할 수 있다.
	ABAnim->OnNextAttackCheck.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn) {
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});

	//GetUserWidgetObject메서드를 PostInitializeComponents에서 호출할경우 위젯이 초기화되어있지 않아 nullptr값을 반환하므로 InitWidget을 통해 수동으로 위젯을 초기화 후 호출해준다.
	HPBarWidget->InitWidget();
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget) {
		CharacterWidget->BindCharacterStat(CharacterStat);
	}
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	//APawn::IsPlayerControlled 메서드는 해당 폰이 플레이어에 의해 조종당하고 있는경우에 true값을 반환합니다.
	bIsPlayer = IsPlayerControlled();
	if (bIsPlayer) {
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(nullptr != ABPlayerController);
	}
	else {
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(nullptr != ABAIController);
	}

	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (bIsPlayer) {
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(nullptr != ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex();
	}
	else {
		//NPC일경우 에셋을 랜덤 선택
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	//RequestAsyncLoad메서드는 FStreamableManager에게 비동기 에셋 로딩을 명령합니다.
	//이때 로드할 에셋의 FSofObjectPath와 FStreamableDelegate의 형태로 메서드를 넘겨주어, 에셋 로딩이 끝났을때 메서드를 매핑할 수 있습니다.
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);
	/*
	FName WeaponSocket(TEXT("hand_rSocket"));
	//SpawnActor메서드는 AActor클래스를 상속받는 액터를 현재 월드에 생성하는 메서드로, 제네릭으로 인자의 클래스를, 인자로 좌표와 회전을 받는다.
	auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (nullptr != CurWeapon) {
		CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}
	*/
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(DefaultContext, 0);
	}
}

int32 AABCharacter::GetExp() const {
	return CharacterStat->GetDropExp();
}

void AABCharacter::SetCharacterState(ECharacterState NewState) {
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState) {
	case ECharacterState::LOADING: {
		if (bIsPlayer) {
			//이 엑터를 해당 컨트롤러에 의해 움직이는 인풋의 스택에서 제거합니다.
			DisableInput(ABPlayerController);

			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			//ABGameMode에서 PlayerStateClass를 ABPlayerState로 지정했으므로 GetPlayerState메서드를 통해 바로 들고 올 수 있다.
			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		else {
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
			ABCHECK(nullptr != ABGameMode);
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}
		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	case ECharacterState::READY: {
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		CharacterStat->OnHPIsZero.AddLambda([this]()->void {
			SetCharacterState(ECharacterState::DEAD);
			});
		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer) {
			SetControllerMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(ABPlayerController);
		}
		else {
			SetControllerMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			ABAIController->RunAI();
		}
		break;
	}
	case ECharacterState::DEAD: {
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		if (bIsPlayer) {
			DisableInput(ABPlayerController);
		}
		else {
			ABAIController->StopAI();
		}

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
			if (bIsPlayer) {
				ABPlayerController->ShowResultUI();
			}
			else {
				Destroy();
			}
			}), DeadTimer, false);
		break;
	}
	}
}

ECharacterState AABCharacter::GetCharacterState() const {
	return CurrentState;
}

float AABCharacter::GetFinalAttackRange() const {
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

bool AABCharacter::CanSetWeapon() {
	return true;
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon) {
	ABCHECK(nullptr != NewWeapon);

	if (nullptr != CurrentWeapon) {
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon) {
		//AttachToComponent메서드는 주어진 액터의 루트 컴포넌트를 인자로 주어진 컴포넌트 하위에 붙입니다.
		//여기서 FAttachmentTransformRules를 통해 컴포넌트를 붙일때 좌표를 어떻게할건지 설정하며,
		//추가적으로 소켓이름의 FName을 넘겨주어 소켓에 붙여넣을 수도 있습니다.
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

//Axis와 Tick모두 매 프레임호출된다
//순서는 Axis -> Tick
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
		AddMovementInput(DirectionToMove);
		break;
	default:
		break;
	}
	DirectionToMove.X = 0;
	DirectionToMove.Y = 0;
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AABCharacter::Look);
		EnhancedInputComponent->BindAction(ViewChangeAction, ETriggerEvent::Triggered, this, &AABCharacter::ViewChange);
		//ACharacter 클래스에는 Jump라는 메서드로 점프 기능이 이미 구현되어 있으므로 이를 이용해 바인드한다.
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacter::Attack);
	}

}

void AABCharacter::Move(const FInputActionValue& NewAxisValue) {
	FVector2D value = NewAxisValue.Get<FVector2D>();
	//AddMovementInput(FVector dir, float scale)은 dir방향으로 scale만큼 이 APawn을 이동시킵니다.
	//FRotationMatrix는 주어진 FRoatator를 회전행렬 바꾸어준다.
	//FRotationMatrix::GetUnitAxis(Exis)는 회전행렬의 회전에 대한 각축의 방향을 반환한다.(FVector)
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), value.Y);
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), value.X);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.X = value.Y;
		DirectionToMove.Y = value.X;
		break;
	default:
		break;
	}
}

void AABCharacter::Look(const FInputActionValue& NewAxisValue) {
	FVector2D value = NewAxisValue.Get<FVector2D>();
	//기본적으로 AController와 APawn의 Yaw는 UseControllerRotationYaw설정으로 인해 Yaw만이 연동되어있다.
	//따라서 아래의 설정만으로는 Pitch가 반영되지 않는다.(반영되더라도 APawn의 Pitch로 재설정되므로 사실상 변화가 없다.)
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerPitchInput(value.Y);
		AddControllerYawInput(value.X);
		break;
	default:
		break;
	}
}

void AABCharacter::ViewChange(const FInputActionValue& NewAxisValue){
	bool value = NewAxisValue.Get<bool>();

	if (value) {
		switch (CurrentControlMode) {
		case EControlMode::GTA:
			GetController()->SetControlRotation(GetActorRotation());
			SetControllerMode(EControlMode::DIABLO);
			break;
		case EControlMode::DIABLO:
			GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
			SetControllerMode(EControlMode::GTA);
			break;
		default:
			break;
		}
	}
}

void AABCharacter::Attack(const FInputActionValue& NewAxisValue) {
	if(IsAttacking) {
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo) {
			IsComboInputOn = true;
		}
	}
	else {
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) {
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();

	OnAttackEnd.Broadcast();
}

//TakeDamage함수 재정의
float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCause) {
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCause);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	/*
	if (FinalDamage > 0.0f) {
		//죽는 모션 실행
		ABAnim->SetDeadAnim();
		//충돌 여부를 false로 변경
		SetActorEnableCollision(false);
	}*/

	CharacterStat->SetDamage(FinalDamage);
	if (CurrentState == ECharacterState::DEAD) {
		if (EventInstigator->IsPlayerController()) {
			auto instigator = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != instigator, 0.0f);
			instigator->NPCKill(this);
		}
	}
	return FinalDamage;
}

//위의 문제를 해결하기 위해 구현할 컨트롤러의 움직임 세팅을 바꾸는 함수를 추가한다.
void AABCharacter::SetControllerMode(EControlMode NewControlMode){

	CurrentControlMode = NewControlMode;
	switch (CurrentControlMode)	
	{
		//GTA식 삼인칭 카메라 컨트롤
		case AABCharacter::EControlMode::GTA:
			//SpringArm->TargetArmLength = 450.0f;
			//회전값을 제로벡터로 초기화
			//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
			ArmLengthTo = 450.0f;	//줄어들 SpringArm의 팔길이 선언
			//bUsePawnControlRotation은 이 SprintArm의 회전이 컨트롤러의 회전을 따라갈 것인가를 정한다. 우리가 사용한 AddControllerPitch/YawInput은 컨트롤러의 회전을 주는것이므로 true로 설정한다.
			SpringArm->bUsePawnControlRotation = true;
			//상위 구성요소의 Pitch/Yaw/Roll를 상속받아 이에대한 상대적인 Rotation으로 나타낼것인지 정합니다.
			SpringArm->bInheritPitch = true;
			SpringArm->bInheritRoll = true;
			SpringArm->bInheritYaw = true;
			//충돌을 계산하여 카메라와 캐릭터 사이의 차폐물을 감지해 카메라를 당길건지에 대한 여부를 정합니다.
			SpringArm->bDoCollisionTest = true;
			//문제가 되었던 설정을 꺼두어 컨트롤러와 캐릭터의 로테이션 연동을 끊는다.
			bUseControllerRotationYaw = false;
			//CharacterMovement::bOrientRotationToMovement는 true일경우 캐릭터가 자기가 움직이는 방향으로 회전합니다.
			GetCharacterMovement()->bOrientRotationToMovement = true;
			//RotationRate를 속도로 하여 부드럽게 회전합니다.
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			//RotationRate는 캐릭터의 회전속도를 설정합니다. 우리는 Roll축(z축) 회전만을 사용할 것이기 때문에 Roll에만 속도를 주었습니다.
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			break;
		//Diablo식 삼인칭 카메라 컨트롤
		case AABCharacter::EControlMode::DIABLO:
			//SpringArm->TargetArmLength = 800.0f;
			//회전값 초기화
			//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
			ArmLengthTo = 800.0f;
			ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);	//목표 SpringArm회전각 설정
			//SpringArm이 컨트롤러의 회전을 따라가지 않습니다.
			SpringArm->bUsePawnControlRotation = false;
			//Diablo식 3인칭 카메라 컨트롤에서는 카메라가 컨트롤러를 따라 회전하지 않으므로 Pawn에 대한 상대적 회전이 아닌 절대 회전으로 나타냅니다.
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritRoll = false;
			SpringArm->bInheritYaw = false;
			//카메라의 충돌검사를 하지 않습니다. 개발단계에서 레벨디자인적으로 해결을 볼 수 있기 때문입니다.
			SpringArm->bDoCollisionTest = false;
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			//Yaw를 폰과 연동합니다.
			bUseControllerRotationYaw = true;
			break;
		case AABCharacter::EControlMode::NPC:
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
			break;
		default:
			break;
	}
}

float AABCharacter::GetFinalAttackDamage() const {
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.0f;
	return AttackDamage * AttackModifier;
}

void AABCharacter::AttackStartComboState() {
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState() {
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck() {
	float FinalAttackRange = GetFinalAttackRange();

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	//SweepSingleByChannel(HitResult,Start,End,Rot,TraceChannel,CollisionShape,Params,ResponseParams):기본 도형을 인자로 받은후 Start부터 End까지 쓸면서 해당영역 내에 물리판정이 일어났는지 조사하여, bool형태로 출력하며, 탐지된 경우 관련정보를 HitResult에 담는다.
	//FHitResult::Result : 충돌발생시 그 정보를 담을 구조체
	//FVector::Start : 시작지점
	//FVector::End : 종료지점
	//FQuat::Rot : 도형의 회전값
	//ECollisionChannel::TraceChannel : 충돌 판정에 사용할 트레이스채널
	//ECollisionShape::CollisionShape : 충돌 판정에 사용할 도형(콜라이더)
	//FCollisionQueryParams::Params : 탐색 방법에 대한 설정값
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params
	);
#if ENABLE_DRAW_DEBUG
	//감지할 범위
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	//감지할 범위의 중심:디버깅용 콜라이더의 월드좌표로 사용
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	//콜라이더의 높이의 반
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	//콜라이더 로테이션
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	//색상, 충돌이 감지될경우 FColor::Green으로, 그렇지 않으면 FColor::Red로 표시
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	//디버깅 오브젝트가 유지되는 시간
	float DebugLifeTime = 5.0f;

	//원형 오브젝트로 쓸면서 공격을 탐지하기때문에 캡슐 콜라이더를 디버깅 오브젝트로 사용.
	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);
#endif
	//충돌이 감지되었을경우 실행
	if (bResult) {
		if (HitResult.GetActor()) {
			ABLOG(Warning, TEXT("Hit Actor name : %s"), *HitResult.GetActor()->GetName());

			// Apply damage
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	if (IsPlayerControlled()) {
		SetControllerMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else {
		SetControllerMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

void AABCharacter::OnAssetLoadCompleted() {
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetCharacterState(ECharacterState::READY);
}