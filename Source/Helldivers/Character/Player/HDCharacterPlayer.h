// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Interface/HDWeaponInterface.h"
#include "HDCharacterPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHDCombatComponent;
class UHDMovementStateComponent;
class UHDStratagemComponent;
class UHDCharacterControlData;
struct FGameplayEventData;
enum class EHDCharacterMovementState : uint8;
enum class EHDCharacterStanceState : uint8;

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface
{
	GENERATED_BODY()

public:
    AHDCharacterPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void									SetCharacterControlData(UHDCharacterControlData* CharacterControlData);

protected:
	virtual void							SetDead() override final;
    virtual void							Tick(float DeltaTime) override final;
	virtual void							PossessedBy(AController* NewController) override final;
    virtual void                            SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override final;

	// RagdollInterface
	virtual void							SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override final;

	// WeaponInterface
    virtual void                            EquipWeapon(AHDWeapon* NewWeapon) override final;
    virtual void                            SpawnProjectile() override final;
    virtual UAnimMontage*                   GetCombatMontage(const EHDCombatMontage MontageType) const override final;
    virtual void                            PlayWeaponMontage(const EHDCombatMontage MontageType) override final;
    virtual bool                            IsEquippedWeapon() const override final;
    virtual bool                            IsShoulder() const override final;
    virtual bool                            IsWeaponAutoFire() const override final;
	virtual float                           GetWeaponFireDelay() const override final;
    virtual void							SetWeaponActive(const bool bActive) override final;

    void                                    SpawnDefaultWeapon(const FGameplayEventData* Payload);

	// CharacterMovementInterface
	virtual void							SetShouldering(const bool bSetAiming) override final;
	virtual void							SetMovementState(const EHDCharacterMovementState NewState) override;
	virtual void							SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	virtual void							RestoreStanceState() override;

    void                                    OnStratagemEventReceived(const FGameplayEventData* Payload);
	void							        TryHoldStratagem();

    void    								UpdateSpeed(const EHDCharacterStanceState StanceState, const EHDCharacterMovementState MoveState);
    
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
	TObjectPtr<UHDMovementStateComponent>	MovementStateComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDStratagemComponent>		Stratagem;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect>            WeaponAttributeSetEffect;
};
