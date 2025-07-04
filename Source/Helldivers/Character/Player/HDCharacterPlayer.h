// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterBase.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "InputActionValue.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Interface/HDCharacterCommandInterface.h"
#include "Interface/HDWeaponInterface.h"
#include "AbilitySystem/Struct/HDTaggedInputAction.h"
#include "HDCharacterPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHDCombatComponent;
class UHDInputActionComponent;
class UHDStratagemComponent;
class AHDWeapon;
class AHDStratagem;
class UHDCharacterControlData;

UENUM(BlueprintType)
enum class EHDCharacterInputAction : uint8
{
	ThirdLook,
	ThirdMove,
	FirstLook,
	FirstMove,
	ChangeControl,
	Count
};

USTRUCT(BlueprintType)
struct FTagEventBindInfo
{
    GENERATED_BODY()

public:
	FTagEventBindInfo()
		: BindFunctionName(FName())
		, InputAction(nullptr)
		, EventConditionTag(FGameplayTag())
	{
	};

    UPROPERTY(EditDefaultsOnly)
    FName						BindFunctionName;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>	InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag				EventConditionTag;
};

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface, public IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	explicit								AHDCharacterPlayer();

    void                                    ChangeCharacterControlType();

protected:
	virtual void							SetDead() override final;
    virtual void							Tick(float DeltaTime) override;
	virtual void							PossessedBy(AController* NewController) override;

    virtual void							SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override final;
    void                                    SetupAbilitySystemInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
    void                                    SetupEventAbilitySystemInputComponent(UEnhancedInputComponent* EnhancedInputComponent);

    void		                            AbilityInputTriggered(const FGameplayTag Tag);
    void		                            AbilityInputReleased(const FGameplayTag Tag);
    void		                            AbilityInputToggled(const FGameplayTag Tag);

	// RagdollInterface
	virtual void							SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override final;

	// WeaponInterface
	virtual AHDWeapon*						GetWeapon() const override final;
	virtual const FVector&					GetHitTarget() const override final;
	virtual const EHDCombatState			GetCombatState() const override final;
	virtual void							Attack(const bool bActive) override final;
	virtual const bool						FireFinished() override final;
    virtual void							SetWeaponActive(const bool bActive) override final;
	virtual const float						Reload() override final;
	virtual void							ReloadFinished() override;

	void									SetCombatState(const EHDCombatState State);
	const float 							Fire(const bool IsPressed);

	// CharacterMovementInterface
	virtual const float						GetAimOffset_Yaw() const override final;
	virtual const float						GetAimOffset_Pitch() const override final;
    virtual const bool						IsShouldering() const override final;
	virtual void							SetShouldering(const bool bSetAiming) override final;
	virtual const bool						IsCharacterLookingViewport() const override final;
	virtual const EHDTurningInPlace			GetTurningInPlace() const override final;
	virtual const bool						IsUseRotateBone() const override final;
	virtual const EHDCharacterMovementState	GetMovementState() const override final;
	virtual void							SetMovementState(const EHDCharacterMovementState NewState) override;
	virtual const EHDCharacterStanceState	GetStanceState() const override;
	virtual void							SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	virtual void							RestoreStanceState() override;

	// CharacterCommandInterface
	virtual void							DetachStratagemWhileThrow() override final;
	virtual void							TryHoldStratagem() override final;
	void									CancleStratagem();

	void									SetCharacterControlData(UHDCharacterControlData* CharacterControlData);

	UHDStratagemComponent*					GetStratagemComponent();
    const float								GetMoveSpeedByState(const EHDCharacterStanceState StanceState, const EHDCharacterMovementState MoveState);
        
	UFUNCTION()
	void									InputStratagemCommand(const FInputActionValue& Value);
	
private:
    void                                    InitAbilitySystemComponent();
	void									InterpFOV(float DeltaSeconds);

    void									ThirdPersonLook(const FInputActionValue& Value);
    void									ThirdPersonMove(const FInputActionValue& Value);
    void									FirstPersonLook(const FInputActionValue& Value);
    void									FirstPersonMove(const FInputActionValue& Value);
    void									SetCharacterControl(const EHDCharacterControlType NewCharacterControlType);

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent>			SpringArm;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent>			FollowCamera;
			
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDCombatComponent>			Combat;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDInputActionComponent>		InputAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDStratagemComponent>		Stratagem;
    
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FTaggedInputAction>				TaggedHoldActions;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FTaggedInputAction>				TaggedToggleActions;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FGameplayTagContainer					EventCallTags;

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FTagEventBindInfo>				TagEventBindInfoList;
};
