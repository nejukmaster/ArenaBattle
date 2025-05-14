// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABCamera.h"
#include "ABAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
//�𸮾������� �����ϴ� ����� ���� ����� ����� ��� ����� ���� "DrawDebugHelpers.h"������� �߰�
#include "DrawDebugHelpers.h"
//DamageEvent�� ����� ����� �����Ͽ�, ��ũ ���� �ذ�
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

	//ACharacter���¿����� ACharacer������� ĸ�� ������Ʈ�� �ڵ����� �����ϱ� ������ ���� �����ϴ� ������ �ʿ����.
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	//HP�� UI�� ĳ���͸� ����ٴϸ� �Ӹ� ���ʿ� ǥ�õǾ�ߵǹǷ� �޽��� ���� ������Ʈ�� �������ش�.
	HPBarWidget->SetupAttachment(GetMesh());

	//ACharacter������ USkeletalMeshComponent�� �ڵ����� �����ϱ� ������ �̸� ���� ������ �ʿ�� ����.
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	//USpringArmComponent���� �̸� ���ǵǾ������Ƿ� �̸� ����Ѵ�.
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
	//USkeletalMeshComponent�� DoesSocketExist�޼���� �־��� �̸��� ���� ������ �Ž��� �����ϴ��� �˻��մϴ�.
	if (GetMesh()->DoesSocketExist(WeaponSocket)) {
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
		if (SK_WEAPON.Succeeded()) {
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}
		//SetupAttachment�޼���� �ش� USkeletalMeshComponent�� �־��� ���Ͽ� ���Դϴ�.
		//���ڷδ� �޽��� ������ �̸��� �޽��ϴ�.
		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	SetControllerMode(EControlMode::DIABLO);

	GetCharacterMovement()->JumpZVelocity = 800.0f;

	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	//�ݸ��� �������� �ռ� ������ "ABCharacter"���������� �������ش�.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 80.0f;
	AttackRadius = 50.0f;

	//HP���� ��ġ�� ĳ���� �Ӹ����� ����
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	//SetWidgetSpace�޼���� ������ � ���·� �������� ���Ѵ�.
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
	//�ִϸ��̼� �ν��Ͻ��� ConstructorHelpers�� ���� �ʱ�ȭ ���� �����Ƿ� ��Ÿ�� �޼����� PostInitializeComponents������ �ʱ�ȭ �Ͽ� ����մϴ�.
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	//��������Ʈ�� Ư�� ��ü�� �ؾ��� ������ �ٸ� ��ü�� ��� ó�����ִ� ������ ���ϸ�, ��������Ʈ�� �Լ��� ����ϸ� ��������Ʈ���� Ư����Ȳ�� �Լ��� ȣ���ϴ� �������� �����ȴ�.
	//�츮�� OnAttackMontageEnded�� ����Ͽ� ��Ÿ���� ����� ������ ��������Ʈ�� �̸� ���� �� ��ü�� �˷��ִ� �������� ������ �����Ѵ�.
	//�𸮾󿡼��� ��Ÿ�� ����� ������ �ߵ��ϴ� OnMontageEnded��� ��������Ʈ�� �����Ѵ�.
	//AddDynamic�� ��������Ʈ�� �Լ��� ������ش�. ������ٶ� this Ű���带 ���� ����� ������Ʈ�� ������ ���� �Ѱ��־�� �Ѵ�.
	//���� ����ϴ� �Լ��� �ݵ�� UFUNCTION�̾�߸� �Ѵ�.
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	//[](���ټҰ���):���ٽĿ��� ���Ǵ� []��ȣ�� ���ټҰ��ڷ� �ش� ���ٽ��� ����� ȯ���� ������ �� �ִ�.
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

	//GetUserWidgetObject�޼��带 PostInitializeComponents���� ȣ���Ұ�� ������ �ʱ�ȭ�Ǿ����� �ʾ� nullptr���� ��ȯ�ϹǷ� InitWidget�� ���� �������� ������ �ʱ�ȭ �� ȣ�����ش�.
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

	//APawn::IsPlayerControlled �޼���� �ش� ���� �÷��̾ ���� �������ϰ� �ִ°�쿡 true���� ��ȯ�մϴ�.
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
		//NPC�ϰ�� ������ ���� ����
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	//RequestAsyncLoad�޼���� FStreamableManager���� �񵿱� ���� �ε��� ����մϴ�.
	//�̶� �ε��� ������ FSofObjectPath�� FStreamableDelegate�� ���·� �޼��带 �Ѱ��־�, ���� �ε��� �������� �޼��带 ������ �� �ֽ��ϴ�.
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);
	/*
	FName WeaponSocket(TEXT("hand_rSocket"));
	//SpawnActor�޼���� AActorŬ������ ��ӹ޴� ���͸� ���� ���忡 �����ϴ� �޼����, ���׸����� ������ Ŭ������, ���ڷ� ��ǥ�� ȸ���� �޴´�.
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
			//�� ���͸� �ش� ��Ʈ�ѷ��� ���� �����̴� ��ǲ�� ���ÿ��� �����մϴ�.
			DisableInput(ABPlayerController);

			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			//ABGameMode���� PlayerStateClass�� ABPlayerState�� ���������Ƿ� GetPlayerState�޼��带 ���� �ٷ� ��� �� �� �ִ�.
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
		//AttachToComponent�޼���� �־��� ������ ��Ʈ ������Ʈ�� ���ڷ� �־��� ������Ʈ ������ ���Դϴ�.
		//���⼭ FAttachmentTransformRules�� ���� ������Ʈ�� ���϶� ��ǥ�� ����Ұ��� �����ϸ�,
		//�߰������� �����̸��� FName�� �Ѱ��־� ���Ͽ� �ٿ����� ���� �ֽ��ϴ�.
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

//Axis�� Tick��� �� ������ȣ��ȴ�
//������ Axis -> Tick
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
		//ACharacter Ŭ�������� Jump��� �޼���� ���� ����� �̹� �����Ǿ� �����Ƿ� �̸� �̿��� ���ε��Ѵ�.
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacter::Attack);
	}

}

void AABCharacter::Move(const FInputActionValue& NewAxisValue) {
	FVector2D value = NewAxisValue.Get<FVector2D>();
	//AddMovementInput(FVector dir, float scale)�� dir�������� scale��ŭ �� APawn�� �̵���ŵ�ϴ�.
	//FRotationMatrix�� �־��� FRoatator�� ȸ����� �ٲپ��ش�.
	//FRotationMatrix::GetUnitAxis(Exis)�� ȸ������� ȸ���� ���� ������ ������ ��ȯ�Ѵ�.(FVector)
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
	//�⺻������ AController�� APawn�� Yaw�� UseControllerRotationYaw�������� ���� Yaw���� �����Ǿ��ִ�.
	//���� �Ʒ��� ���������δ� Pitch�� �ݿ����� �ʴ´�.(�ݿ��Ǵ��� APawn�� Pitch�� �缳���ǹǷ� ��ǻ� ��ȭ�� ����.)
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

//TakeDamage�Լ� ������
float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCause) {
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCause);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	/*
	if (FinalDamage > 0.0f) {
		//�״� ��� ����
		ABAnim->SetDeadAnim();
		//�浹 ���θ� false�� ����
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

//���� ������ �ذ��ϱ� ���� ������ ��Ʈ�ѷ��� ������ ������ �ٲٴ� �Լ��� �߰��Ѵ�.
void AABCharacter::SetControllerMode(EControlMode NewControlMode){

	CurrentControlMode = NewControlMode;
	switch (CurrentControlMode)	
	{
		//GTA�� ����Ī ī�޶� ��Ʈ��
		case AABCharacter::EControlMode::GTA:
			//SpringArm->TargetArmLength = 450.0f;
			//ȸ������ ���κ��ͷ� �ʱ�ȭ
			//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
			ArmLengthTo = 450.0f;	//�پ�� SpringArm�� �ȱ��� ����
			//bUsePawnControlRotation�� �� SprintArm�� ȸ���� ��Ʈ�ѷ��� ȸ���� ���� ���ΰ��� ���Ѵ�. �츮�� ����� AddControllerPitch/YawInput�� ��Ʈ�ѷ��� ȸ���� �ִ°��̹Ƿ� true�� �����Ѵ�.
			SpringArm->bUsePawnControlRotation = true;
			//���� ��������� Pitch/Yaw/Roll�� ��ӹ޾� �̿����� ������� Rotation���� ��Ÿ�������� ���մϴ�.
			SpringArm->bInheritPitch = true;
			SpringArm->bInheritRoll = true;
			SpringArm->bInheritYaw = true;
			//�浹�� ����Ͽ� ī�޶�� ĳ���� ������ ������ ������ ī�޶� �������� ���� ���θ� ���մϴ�.
			SpringArm->bDoCollisionTest = true;
			//������ �Ǿ��� ������ ���ξ� ��Ʈ�ѷ��� ĳ������ �����̼� ������ ���´�.
			bUseControllerRotationYaw = false;
			//CharacterMovement::bOrientRotationToMovement�� true�ϰ�� ĳ���Ͱ� �ڱⰡ �����̴� �������� ȸ���մϴ�.
			GetCharacterMovement()->bOrientRotationToMovement = true;
			//RotationRate�� �ӵ��� �Ͽ� �ε巴�� ȸ���մϴ�.
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			//RotationRate�� ĳ������ ȸ���ӵ��� �����մϴ�. �츮�� Roll��(z��) ȸ������ ����� ���̱� ������ Roll���� �ӵ��� �־����ϴ�.
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			break;
		//Diablo�� ����Ī ī�޶� ��Ʈ��
		case AABCharacter::EControlMode::DIABLO:
			//SpringArm->TargetArmLength = 800.0f;
			//ȸ���� �ʱ�ȭ
			//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
			ArmLengthTo = 800.0f;
			ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);	//��ǥ SpringArmȸ���� ����
			//SpringArm�� ��Ʈ�ѷ��� ȸ���� ������ �ʽ��ϴ�.
			SpringArm->bUsePawnControlRotation = false;
			//Diablo�� 3��Ī ī�޶� ��Ʈ�ѿ����� ī�޶� ��Ʈ�ѷ��� ���� ȸ������ �����Ƿ� Pawn�� ���� ����� ȸ���� �ƴ� ���� ȸ������ ��Ÿ���ϴ�.
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritRoll = false;
			SpringArm->bInheritYaw = false;
			//ī�޶��� �浹�˻縦 ���� �ʽ��ϴ�. ���ߴܰ迡�� ���������������� �ذ��� �� �� �ֱ� �����Դϴ�.
			SpringArm->bDoCollisionTest = false;
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			//Yaw�� ���� �����մϴ�.
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
	//SweepSingleByChannel(HitResult,Start,End,Rot,TraceChannel,CollisionShape,Params,ResponseParams):�⺻ ������ ���ڷ� ������ Start���� End���� ���鼭 �ش翵�� ���� ���������� �Ͼ���� �����Ͽ�, bool���·� ����ϸ�, Ž���� ��� ���������� HitResult�� ��´�.
	//FHitResult::Result : �浹�߻��� �� ������ ���� ����ü
	//FVector::Start : ��������
	//FVector::End : ��������
	//FQuat::Rot : ������ ȸ����
	//ECollisionChannel::TraceChannel : �浹 ������ ����� Ʈ���̽�ä��
	//ECollisionShape::CollisionShape : �浹 ������ ����� ����(�ݶ��̴�)
	//FCollisionQueryParams::Params : Ž�� ����� ���� ������
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
	//������ ����
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	//������ ������ �߽�:������ �ݶ��̴��� ������ǥ�� ���
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	//�ݶ��̴��� ������ ��
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	//�ݶ��̴� �����̼�
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	//����, �浹�� �����ɰ�� FColor::Green����, �׷��� ������ FColor::Red�� ǥ��
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	//����� ������Ʈ�� �����Ǵ� �ð�
	float DebugLifeTime = 5.0f;

	//���� ������Ʈ�� ���鼭 ������ Ž���ϱ⶧���� ĸ�� �ݶ��̴��� ����� ������Ʈ�� ���.
	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);
#endif
	//�浹�� �����Ǿ������ ����
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