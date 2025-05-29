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
	virtual void							BeginPlay() override;
    virtual void							SetDead() override;
    virtual void							Tick(float DeltaTime) override;
	virtual void							PossessedBy(AController* NewController) override;

	// WeaponInferface
	virtual void							EquipWeapon(AHDWeapon* NewWeapon) override final;
	virtual AHDWeapon*						GetWeapon() const override final;
	virtual const FVector&					GetHitTarget() const override final;
	virtual const EHDCombatState			GetCombatState() const override final;
    virtual void							Fire(const bool IsPressed);
    virtual void							SetWeaponActive(const bool bActive) override final;


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

	virtual void							SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override final;
	virtual const float						GetRagdollPysicsLinearVelocity() const override final;

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

	UFUNCTION()
	void									OnTurningTimelineUpdate(const float Value);

	void									CalculationAimOffset_Pitch();

	void									SpawnDefaultWeapon();

	void									InterpFOV(float DeltaSeconds);

	void									PlayFireMontage(const bool bAiming);
	void									PlayThrowMontage();

	void									RagdollCameraCapsuleSync();

protected:
	// Camera Section
	UPROPERTY()
	TObjectPtr<USpringArmComponent>			CameraBoom;

	UPROPERTY()
	TObjectPtr<UCameraComponent>			FollowCamera;
			
	UPROPERTY()
	TArray<EHDCommandInput>					CurrentInputCommandList;

	// Combat
	UPROPERTY()
	TObjectPtr<UHDCombatComponent>			Combat;

	UPROPERTY()
	TObjectPtr<AHDWeapon>					Weapon;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AHDWeapon>					DefaultWeaponClass;
	
	UPROPERTY(EditAnywhere, Category = "CharacterControl")
	TObjectPtr<UCurveFloat>					DefaultCurve;

private:
	FRotator								StartingAimRotation;

	float									AimOffset_Yaw;
	float									InterpAimOffset_Yaw;
	float									AimOffset_Pitch;

	bool									bIsSprint;

	bool									bIsCharacterLookingViewport;
	bool									bUseRotateRootBone;

	UPROPERTY(EditAnywhere, Category = "Input|TurnInPlace")
	float									TurnThreshold;

	EHDTurningInPlace						TurningInPlace;
	FTimeline								TurningTimeline;

	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TSubclassOf<AHDStratagem>				StratagemClass;

	UPROPERTY()
	TObjectPtr<AHDStratagem>				Stratagem;
	
	FName									SelectedStratagemName;

	float									SelecteddStratagemActiveDelay;
	
	UPROPERTY()
	TArray<FName>							CommandMatchStratagemNameList;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TObjectPtr<UDataTable>					AvaliableStratagemDataTable;

	// HUD, Crosshair
	float									DefaultFOV;

	FTimerHandle							RagdollTimerHandle;
};
