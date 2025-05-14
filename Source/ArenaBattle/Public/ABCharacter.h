// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ABCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndEDelegate);

//언리얼에서 제공하는 인간형 폰의 에셋 Character는 공통적으로 ACharacter클래스를 상속받고 있음을 알 수 있다.
//실제로 ACharacter의 헤더를 열어보게 되면 APawn클래스를 상속받고있는것을 확인할 수 있다.
UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacter();
	void SetCharacterState(ECharacterState NewState);
	ECharacterState GetCharacterState() const;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PostInitializeComponents는 모든 Components가 초기화된 후에 호출됩니다.
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//TakeDamage는 언리얼에서 AActor에 구현해놓은 데미지 프레임워크이다.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCause) override;

	//PossessedBy는 폰이 컨트롤러에 의해 빙의 당했을때, 오직 서버에서만 호출되는 메서드이다.
	virtual void PossessedBy(AController* NewController) override;

	bool CanSetWeapon();
	void SetWeapon(class AABWeapon* NewWeapon);

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class AABWeapon* CurrentWeapon;

	//ABPawn에서 구현했던 동일한 액터를 ACharacter 기반으로 구현합니다.
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* CharacterStat;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	void Attack(const FInputActionValue& NewAxisValue);
	FOnAttackEndEDelegate OnAttackEnd;

	int32 GetExp() const;
	float GetFinalAttackRange() const;
	float GetFinalAttackDamage() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode {
		GTA,
		DIABLO,
		NPC
	};
	//카메라 조작법을 주어진 인수에 따라 변경할 SetControllerMode함수 작성
	//GTA방식의 삼인칭 조작법을 0번 모드에 매핑한다.
	void SetControllerMode(EControlMode NewControlMode);
	
	EControlMode CurrentControlMode = EControlMode::GTA;
	//각 축의 입력을 종합해 보관할 벡터변수
	//UPROPERTY를 사용하지 않는 변수는 초기값을 지정해주는게 안전하다.
	FVector DirectionToMove = FVector::ZeroVector;

	//시점 변환시 SpringArm의 팔 길이를 자연스럽게 줄일때 사용할 변수입니다.
	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

private:
	void Move(const FInputActionValue& NewAxisValue);

	void Look(const FInputActionValue& NewAxisValue);

	void ViewChange(const FInputActionValue& NewAxisValue);

	void OnAssetLoadCompleted();

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* LookAction;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ViewChangeAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* AttackAction;

	//UFUNCTION으로 지정한 함수는 블루프린트에서 호출및 수정이 가능합니다.
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();
	void AttackCheck();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY()
	class UABAnimInstance* ABAnim;

	int32 AssetIndex = 0;
	//FSoftObjectPath는 비동기 로딩할 에셋의 전체 이름의 문자열이 들어가있는 구조체 입니다.
	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	//TSharedPtr는 GameInstance의 FStreamableManager를 활성화 시키기위한 스마트 포인터 입니다.
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	ECharacterState CurrentState;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = true))
	bool bIsPlayer;

	UPROPERTY()
	class AABAIController* ABAIController;

	UPROPERTY()
	class AABPlayerController* ABPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};
};
