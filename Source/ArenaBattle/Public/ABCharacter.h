// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ABCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndEDelegate);

//�𸮾󿡼� �����ϴ� �ΰ��� ���� ���� Character�� ���������� ACharacterŬ������ ��ӹް� ������ �� �� �ִ�.
//������ ACharacter�� ����� ����� �Ǹ� APawnŬ������ ��ӹް��ִ°��� Ȯ���� �� �ִ�.
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

	//PostInitializeComponents�� ��� Components�� �ʱ�ȭ�� �Ŀ� ȣ��˴ϴ�.
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//TakeDamage�� �𸮾󿡼� AActor�� �����س��� ������ �����ӿ�ũ�̴�.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCause) override;

	//PossessedBy�� ���� ��Ʈ�ѷ��� ���� ���� ��������, ���� ���������� ȣ��Ǵ� �޼����̴�.
	virtual void PossessedBy(AController* NewController) override;

	bool CanSetWeapon();
	void SetWeapon(class AABWeapon* NewWeapon);

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class AABWeapon* CurrentWeapon;

	//ABPawn���� �����ߴ� ������ ���͸� ACharacter ������� �����մϴ�.
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
	//ī�޶� ���۹��� �־��� �μ��� ���� ������ SetControllerMode�Լ� �ۼ�
	//GTA����� ����Ī ���۹��� 0�� ��忡 �����Ѵ�.
	void SetControllerMode(EControlMode NewControlMode);
	
	EControlMode CurrentControlMode = EControlMode::GTA;
	//�� ���� �Է��� ������ ������ ���ͺ���
	//UPROPERTY�� ������� �ʴ� ������ �ʱⰪ�� �������ִ°� �����ϴ�.
	FVector DirectionToMove = FVector::ZeroVector;

	//���� ��ȯ�� SpringArm�� �� ���̸� �ڿ������� ���϶� ����� �����Դϴ�.
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

	//UFUNCTION���� ������ �Լ��� �������Ʈ���� ȣ��� ������ �����մϴ�.
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
	//FSoftObjectPath�� �񵿱� �ε��� ������ ��ü �̸��� ���ڿ��� ���ִ� ����ü �Դϴ�.
	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	//TSharedPtr�� GameInstance�� FStreamableManager�� Ȱ��ȭ ��Ű������ ����Ʈ ������ �Դϴ�.
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
