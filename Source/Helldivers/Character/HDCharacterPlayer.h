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
class AHDWeapon;
class AHDStratagem;

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface, public IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	explicit								AHDCharacterPlayer();
	UDataTable*								GetAvaliableStratagemDataTable()						{ return AvaliableStratagemDataTable; }

protected:
    virtual void							SetDead() override;
    virtual void							Tick(float DeltaTime) override;
	virtual void							PossessedBy(AController* NewController) override;

	// RagdollInterface
	virtual void							SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override final;

	// WeaponInferface
	virtual void							EquipWeapon(AHDWeapon* NewWeapon) override final;
	virtual AHDWeapon*						GetWeapon() const override final;
	virtual const FVector&					GetHitTarget() const override final;
	virtual const EHDCombatState			GetCombatState() const override final;
    virtual void							Fire(const bool IsPressed);
    virtual void							SetWeaponActive(const bool bActive) override final;
	virtual void							Reload() override final;

	// CharacterMovementInterface
	virtual const float						GetAimOffset_Yaw() const override final					{ return AimOffset_Yaw; }
	virtual const float						GetAimOffset_Pitch() const override final				{ return AimOffset_Pitch; }

    virtual const bool						IsShouldering() const override final;
	virtual void							SetShouldering(const bool bSetAiming) override;
	virtual const bool						IsCharacterLookingViewport() const override final		{ return bIsCharacterLookingViewport; }

	virtual const EHDTurningInPlace			GetTurningInPlace() const override final				{ return TurningInPlace; }
	virtual const bool						IsUseRotateBone() const override final					{ return bUseRotateRootBone; }

	virtual const bool						IsSprint() const override final							{ return bIsSprint; }
	virtual void							SetSprint(const bool bSprint) override;

	virtual const bool						IsCrouch() const override final							{ return bIsCrouch; }
	virtual void							SetCrouch(const bool bCrouch) override;
	virtual const bool						IsProne() const override final							{ return bIsProne; }
	virtual void							SetProne(const bool bProne) override;

	// CharacterCommandInterface
	virtual AHDStratagem*					GetStratagem() const override final						{ return Stratagem; }
	virtual const FVector&					GetThrowDirection() const override final;
	virtual void							ThrowStratagem() override final;

	// CommandInput
	FORCEINLINE TArray<FName>				GetCommandMatchStratagemNameList() const 				{ return CommandMatchStratagemNameList; }
	FORCEINLINE FName						GetSelectedStraragemName() const						{ return SelectedStratagemName; }
	virtual void							AddStratagemCommand(const EHDCommandInput NewInput);
		
private:
    void									AimOffset(const float DeltaTime);
	void									TurnInPlace(const float DeltaTime);
	void									CalculationAimOffset_Pitch();

	void									SpawnDefaultWeapon();

	void									InterpFOV(float DeltaSeconds);

	void									PlayMontage(UAnimMontage* Montage, const FName SectionName = FName());

	void									FireTimerFinished();
	void									ReloadTimerFinished();

protected:
	// Camera Section
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent>			CameraBoom;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent>			FollowCamera;
			
	UPROPERTY()
	TArray<EHDCommandInput>					CurrentInputCommandList;

	// Combat
	UPROPERTY()
	TObjectPtr<UHDCombatComponent>			Combat;

	UPROPERTY(EditAnywhere, Category = "Player|Weapon")
	TSubclassOf<AHDWeapon>					DefaultWeaponClass;
	
	UPROPERTY(EditAnywhere, Category = "Player|CharacterControl")
	TObjectPtr<UCurveFloat>					DefaultCurve;

private:
	FRotator								StartingAimRotation;

	float									AimOffset_Yaw;
	float									InterpAimOffset_Yaw;
	float									AimOffset_Pitch;

	bool									bIsSprint;
	bool									bIsCrouch;
	bool									bIsProne;

	bool									bIsCharacterLookingViewport;
	bool									bUseRotateRootBone;

	UPROPERTY(EditAnywhere, Category = "Player|Input")
	float									TurnThreshold;

	EHDTurningInPlace						TurningInPlace;

	UPROPERTY(EditAnywhere, Category = "Player|Stratagem")
	TSubclassOf<AHDStratagem>				StratagemClass;

	UPROPERTY()
	TObjectPtr<AHDStratagem>				Stratagem;
	
	FName									SelectedStratagemName;
	float									SelecteddStratagemActiveDelay;
	
	UPROPERTY()
	TArray<FName>							CommandMatchStratagemNameList;
	
	UPROPERTY(EditAnywhere, Category = "Player|Stratagem")
	TObjectPtr<UDataTable>					AvaliableStratagemDataTable;

	// HUD, Crosshair
	float									DefaultFOV;

	FTimerHandle							FireTimer;
	FTimerHandle							ReloadTimer;
};
