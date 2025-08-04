// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Interface/HDCharacterCommandInterface.h"
#include "Interface/HDWeaponInterface.h"
#include "HDCharacterPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHDCombatComponent;
class UHDMovementStateComponent;
class UHDStratagemComponent;
class UHDCharacterControlData;
enum class EHDTurningInPlace : uint8;
enum class EHDCharacterMovementState : uint8;
enum class EHDCharacterStanceState : uint8;

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface, public IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	explicit								AHDCharacterPlayer();

    void									SetCharacterControlData(UHDCharacterControlData* CharacterControlData);

protected:
	virtual void							SetDead() override final;
    virtual void							Tick(float DeltaTime) override;
	virtual void							PossessedBy(AController* NewController) override;

	// RagdollInterface
	virtual void							SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override final;

	// WeaponInterface
    virtual void                            EquipWeapon(AHDWeapon* NewWeapon) override final;
	virtual const float                     GetWeaponFireDelay() const override final;
	virtual void							Attack(const bool bActive) override final;
	virtual const bool						FireFinished() override final;
    virtual void							SetWeaponActive(const bool bActive) override final;
	virtual const float						Reload() override final;
	virtual void							ReloadFinished() override;

	const float 							Fire(const bool IsPressed);

	// CharacterMovementInterface
	virtual void							SetShouldering(const bool bSetAiming) override final;
	virtual void							SetMovementState(const EHDCharacterMovementState NewState) override;
	virtual void							SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	virtual void							RestoreStanceState() override;

	// CharacterCommandInterface
	virtual void							DetachStratagemWhileThrow() override final;
	virtual void							TryHoldStratagem() override final;
	void									CancleStratagem();


	UHDStratagemComponent*					GetStratagemComponent();
    const float								GetMoveSpeedByState(const EHDCharacterStanceState StanceState, const EHDCharacterMovementState MoveState);
        
	UFUNCTION()
	void									InputStratagemCommand(const FInputActionValue& Value);
	
private:
    void                                    InitAbilitySystemComponent();
	void									InterpFOV(float DeltaSeconds);

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent>			SpringArm;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent>			FollowCamera;
			
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDCombatComponent>			Combat;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDMovementStateComponent>	MovementState;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDStratagemComponent>		Stratagem;    
};
