// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDCharacterBase.h"
#include "CharacterTypes/HDArmorTypes.h"
#include "CharacterTypes/HDTurningInPlace.h"
#include "CharacterTypes/HDCombatState.h"
#include "InputActionValue.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Interface/HDWeaponInterface.h"
#include "Interface/HDCharacterCommandInterface.h"
#include "Weapon/WeaponTypes.h"
#include "CharacterTypes/HDCommandTypes.h"
#include "HDCharacterPlayer.generated.h"

class UHDCharacterControlData;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class AHDWeapon;
class AHDStratagem;
class UHDGASPlayerUserWidget;

UCLASS()
class HELLDIVERS_API AHDCharacterPlayer : public AHDCharacterBase, public IHDCharacterMovementInterface, public IHDWeaponInterface, public IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	explicit AHDCharacterPlayer();

	virtual void							SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UDataTable*								GetAvaliableStratagemDataTable()						{ return AvaliableStratagemDataTable; }

protected:
    virtual void							BeginPlay() override;
    virtual void							SetDead() override;
    virtual void							Tick(float DeltaTime) override;

	void									SetArmor(const EHDArmorType NewArmorType);

	// WeaponInferface
	virtual void							EquipWeapon(AHDWeapon* NewWeapon) override final;
	virtual AHDWeapon*						GetWeapon() const override final						{ return Weapon; }
	virtual const FVector&					GetHitTarget() const override final						{ return HitTarget; }
	virtual const EHDCombatState			GetCombatState() const override final					{ return CombatState; }
	virtual void							Fire(const bool IsPressed);
	virtual void							SetWeaponActive(const bool bActive) override final;

	virtual void							SetCharacterControlData(UHDCharacterControlData* CharacterControlData) override final;

	// CharacterMovementInterface
    virtual const float						GetAimOffset_Yaw() const override final					{ return AimOffset_Yaw; }
    virtual const float						GetAimOffset_Pitch() const override final				{ return AimOffset_Pitch; }

    virtual const bool						IsShouldering() const override final					{ return bIsShoulder; }
    virtual void							SetShouldering(const bool bSetAiming) override final	{ bIsShoulder = bSetAiming; }

	virtual const EHDCharacterControlType	GetCharacterControlType() const							{ return CurrentCharacterControlType; }
	virtual void							ChanageCharacterControlType();

	virtual const EHDTurningInPlace			GetTurningInPlace() const override final				{ return TurningInPlace; }
	virtual const bool						IsUseRotateBone() const override final					{ return bUseRotateRootBone; }

	// CharacterCommandInterface
	virtual AHDStratagem*					GetStratagem() const override final						{ return Stratagem; }
	virtual const FVector					GetThrowDirection() const override final				{ return HitTarget; }
	virtual void							ThrowStratagem() override final;

	// CommandInput
	FORCEINLINE TArray<FName>				GetCommandMatchStratagemNameList() const 				{ return CommandMatchStratagemNameList; }
	FORCEINLINE FName						GetSelectedStraragemName() const						{ return SelectedStratagemName; }
	virtual void							AddStratagemCommand(const EHDCommandInput NewInput);
	
private:
    void									AimOffset(float DeltaTime);
	void									TurnInPlace(float DeltaTime);

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
	const bool 								CanFire();
	void									StartFireTimer();
	void									FireTimerFinished();

	void									PlayThrowMontage();
			
protected:
	// Camera Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// Input Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	ChangeControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	ThirdPersonMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	ThirdPersonLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	FirstPersonMoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	FirstPersonLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	ShoulderAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	SkillAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	ThrowGrenadeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	StratagemModeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	InputCommandAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>	ThrowStratagemAction;
	
	EHDCharacterControlType		CurrentCharacterControlType;
	EHDArmorType				ArmorType = EHDArmorType::Medium;
	
	UPROPERTY(VisibleAnywhere, Category = Stratagem, Meta = (AllowPrivateAccess = "true"))
	TArray<EHDCommandInput>		CurrentInputCommandList;

private:
	float						ViewportAimOffset_Yaw	= 0.f;
    FRotator					StartingAimRotation		= FRotator();
    float						AimOffset_Yaw			= 0.f;
    float						AimOffset_Pitch			= 0.f;
	float						InterpAimOffset_Yaw		= 0.f;
    bool						bIsShoulder				= false;
	bool						bUseRotateRootBone		= false;
	EHDTurningInPlace			TurningInPlace			= EHDTurningInPlace::NotTurning;
	EHDCombatState				CombatState				= EHDCombatState::Unoccupied;

	bool						bCanFire				= true;
	bool						bIsFireButtonPressed	= false;
		
	UPROPERTY(EditAnywhere, Category = Weapon, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHDWeapon>		DefaultWeaponClass;

	UPROPERTY(EditAnywhere, Category = Weapon, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AHDWeapon>		Weapon;
	
	EWeaponType					WeaponType				= EWeaponType::Count;
	FVector						HitTarget				= FVector();
	FTimerHandle				FireTimer;
	
	UPROPERTY(EditAnywhere, Category = Stratagem, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHDStratagem>	StratagemClass;
	
	UPROPERTY(EditAnywhere, Category = Stratagem, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AHDStratagem>	Stratagem;
	
	UPROPERTY(EditAnywhere, Category = Stratagem, Meta = (AllowPrivateAccess = "true"))
	FName						SelectedStratagemName;

	float						SelecteddStratagemActiveDelay;
	
	UPROPERTY(EditAnywhere, Category = Stratagem, Meta = (AllowPrivateAccess = "true"))
	TArray<FName>				CommandMatchStratagemNameList;

	TObjectPtr<UDataTable>		AvaliableStratagemDataTable;
	FTimerHandle				ThrowTimer;

	// HUD, Crosshair
	float						DefaultFOV				= 55.f;

	UPROPERTY(EditAnywhere, Category = FOV)
	float						ZoomedFOV				= 30.f;

	float						CurrentFOV				= 0.f;

	UPROPERTY(EditAnywhere, Category = FOV)
	float						ZoomInterpSpeed			= 20.f;

    float						ErgonomicFactor			= 0.f;
};
