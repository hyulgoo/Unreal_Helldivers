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
#include "HDCharacterPlayer.generated.h"

class UHDCharacterControlData;
class USpringArmComponent;
class UCameraComponent;
class UHDCombatComponent;
class UInputAction;
class AHDWeapon;
class AHDStratagem;
class UHDGASPlayerUserWidget;

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface, public IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	explicit								AHDCharacterPlayer();

	virtual void							SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UDataTable*								GetAvaliableStratagemDataTable()						{ return AvaliableStratagemDataTable; }

protected:
	virtual void							PostInitializeComponents() override;
    virtual void							SetDead() override;
    virtual void							Tick(float DeltaTime) override;

	// WeaponInferface
	virtual void							EquipWeapon(AHDWeapon* NewWeapon) override final;
	virtual AHDWeapon*						GetWeapon() const override final;
	virtual const FVector&					GetHitTarget() const override final;
	virtual const EHDCombatState			GetCombatState() const override final;
    virtual void							Fire(const bool IsPressed);
    virtual void							SetWeaponActive(const bool bActive) override final;

	virtual void							SetCharacterControlData(UHDCharacterControlData* CharacterControlData) override final;

	// CharacterMovementInterface
	virtual const float						GetAimOffset_Yaw() const override final					{ return AimOffset_Yaw; }
	virtual const float						GetAimOffset_Pitch() const override final				{ return AimOffset_Pitch; }

    virtual const bool						IsShouldering() const override final;
    virtual void							SetShouldering(const bool bSetAiming) override final;
	virtual const bool						IsCharacterLookingViewport() const override final		{ return bIsCharacterLookingViewport; }

	virtual const EHDCharacterControlType	GetCharacterControlType() const							{ return CurrentCharacterControlType; }
	virtual void							ChangeCharacterControlType();

	virtual const EHDTurningInPlace			GetTurningInPlace() const override final				{ return TurningInPlace; }
	virtual const bool						IsUseRotateBone() const override final					{ return bUseRotateRootBone; }

	virtual const bool						IsSprint() const override final							{ return bIsSprint; }
	virtual void							SetSprint(const bool bSprint) override;

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

	// Character Control Section
	void									SetCharacterControl(const EHDCharacterControlType NewCharacterControlType);

	void									ThirdPersonMove(const FInputActionValue& Value);
	void									ThirdPersonLook(const FInputActionValue& Value);

	void									FirstPersonMove(const FInputActionValue& Value);
	void									FirstPersonLook(const FInputActionValue& Value);

	void									TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void									InterpFOV(float DeltaSeconds);

	void									PlayFireMontage(const bool bAiming);
	void									PlayThrowMontage();
			
protected:
	// Camera Section
	UPROPERTY()
	TObjectPtr<USpringArmComponent>			CameraBoom;

	UPROPERTY()
	TObjectPtr<UCameraComponent>			FollowCamera;

	// Input Section
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				ChangeControlAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				ThirdPersonMoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				ThirdPersonLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				FirstPersonMoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				FirstPersonLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				InputCommandAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>				ThrowStratagemAction;
	
    EHDCharacterControlType					CurrentCharacterControlType;
	
	UPROPERTY(VisibleAnywhere, Category = "Stratagem")
	TArray<EHDCommandInput>					CurrentInputCommandList;

	// Combat
	UPROPERTY()
	TObjectPtr<UHDCombatComponent>			Combat;

	UPROPERTY()
	TObjectPtr<AHDWeapon>					Weapon;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AHDWeapon>					DefaultWeaponClass;

private:
	FRotator								StartingAimRotation;

	float									LastFrame_Yaw;
	float									AimOffset_Yaw;
	float									InterpAimOffset_Yaw;
	float									AimOffset_Pitch;
	float									AimOffsetYawCompensation;

	bool									bIsSprint;

	bool									bIsCharacterLookingViewport;

	UPROPERTY(VisibleAnywhere, Category = "Input|TurnInPlace")
	bool									bUseRotateRootBone;

	UPROPERTY(EditAnywhere, Category = "Input|TurnInPlace")
	float									TurnThreshold;


	EHDTurningInPlace						TurningInPlace;
		
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TSubclassOf<AHDStratagem>				StratagemClass;

	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TObjectPtr<AHDStratagem>				Stratagem;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TObjectPtr<AHDStratagem>				Info;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	FName									SelectedStratagemName;

	float									SelecteddStratagemActiveDelay;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TArray<FName>							CommandMatchStratagemNameList;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TObjectPtr<UDataTable>					AvaliableStratagemDataTable;

	FTimerHandle							ThrowTimer;

	// HUD, Crosshair
	float									DefaultFOV;
};
