// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDCharacterBase.h"
#include "CharacterTypes/HDCharacterStateTypes.h"
#include "InputActionValue.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Interface/HDCharacterCommandInterface.h"
#include "Interface/HDWeaponInterface.h"
#include "Weapon/WeaponTypes.h"
#include "Components/TimelineComponent.h"
#include "HDCharacterPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHDCombatComponent;
class UHDInputActionComponent;
class UHDStratagemComponent;
class AHDWeapon;
class AHDStratagem;
class UHDCharacterControlData;

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface, public IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	explicit								AHDCharacterPlayer();

protected:
	virtual void							SetDead() override final;
    virtual void							Tick(float DeltaTime) override;
	virtual void							PossessedBy(AController* NewController) override;

	// RagdollInterface
	virtual void							SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override final;

	// WeaponInterface
	virtual AHDWeapon*						GetWeapon() const override final;
	virtual const FVector&					GetHitTarget() const override final;
	virtual const EHDCombatState			GetCombatState() const override final;
	void									SetCombatState(const EHDCombatState State);
	virtual void							Attack(const bool bActive) override final;
    const float 							Fire(const bool IsPressed);
	virtual const bool						FireFinished() override final;
    virtual void							SetWeaponActive(const bool bActive) override final;
	virtual const float						Reload() override final;
	virtual void							ReloadFinished() override;

	// CharacterMovementInterface
	virtual const float						GetAimOffset_Yaw() const override final;
	virtual const float						GetAimOffset_Pitch() const override final;
    virtual const bool						IsShouldering() const override final;
	virtual void							SetShouldering(const bool bSetAiming) override final;
	virtual const bool						IsCharacterLookingViewport() const override final;
	virtual const EHDTurningInPlace			GetTurningInPlace() const override final;
	virtual const bool						IsUseRotateBone() const override final;
	virtual const bool						IsSprint() const override final;
	virtual void							SetSprint(const bool bSprint) override;
	virtual const EHDCharacterMovementState	GetCharacterMovementState() const override;
	virtual void							SetCharacterMovementState(const EHDCharacterMovementState NewState, const bool bForced = false);
	virtual void							RestoreMovementState() override;

	// CharacterCommandInterface
	virtual void							DetachStratagemWhileThrow() override final;
	virtual void							TryHoldStratagem() override final;
	void									CancleStratagem();

	void									SetCharacterControlData(UHDCharacterControlData* CharacterControlData);

	UHDStratagemComponent*					GetStratagemComponent();
	
private:
	void									InterpFOV(float DeltaSeconds);

	void									PlayMontage(UAnimMontage* Montage, const FName SectionName = FName());

private:
	// Component
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
};
