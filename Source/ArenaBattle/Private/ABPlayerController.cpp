// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ABCharacter.h"
#include "ABGameplayWidget.h"
#include "MyABGameplayResultWidget.h"
#include "ABGameState.h"

AABPlayerController::AABPlayerController() {
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded()) {
		HUDWidgetClass = UI_HUD_C.Class;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT(TEXT("/Game/ABInputMappingCtx.ABInputMappingCtx"));
	if (DEFAULT_CONTEXT.Succeeded())
		DefaultContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AB_PAUSE(TEXT("/Game/ABPause.ABPause"));
	if (AB_PAUSE.Succeeded())
		PauseAction = AB_PAUSE.Object;

	static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if (UI_MENU_C.Succeeded())
	{
		MenuWidgetClass = UI_MENU_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UMyABGameplayResultWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded()) {
		ResultWidgetClass = UI_RESULT_C.Class;
	}
	ABCHECK(UI_RESULT_C.Succeeded());
}

void AABPlayerController::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AABPlayerController::OnGamePause);
	}
}

void AABPlayerController::OnGamePause() {
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true);
	ChangeInputMode(false);
}

void AABPlayerController::OnPossess(APawn* apawn) {
	ABLOG_S(Warning);
	Super::OnPossess(apawn);
}

void AABPlayerController::BeginPlay() {
	Super::BeginPlay();

	ChangeInputMode(true);

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChangedDelegate.Broadcast();

	ResultWidget = CreateWidget<UMyABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
		SubSystem->AddMappingContext(DefaultContext, 0);
}

void AABPlayerController::ShowResultUI() {
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

void AABPlayerController::ChangeInputMode(bool bGameMode) {
	if (bGameMode) {
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}

	else {
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::AddGameScore() const{
	ABPlayerState->AddGameScore();
}

UABHUDWidget* AABPlayerController::GetHUDWidget() const {
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const {
	ABPlayerState->AddExp(KilledNPC->GetExp());
}