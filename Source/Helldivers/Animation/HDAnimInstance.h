// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "HDAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UHDCombatComponent;
class UHDMovementStateComponent;
enum class EHDCombatState : uint8;

UCLASS()
class HELLDIVERS_API UHDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    explicit								UHDAnimInstance();

protected:
    virtual void							NativeInitializeAnimation() override final;
    virtual void							NativeUpdateAnimation(float DeltaSeconds) override final;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TObjectPtr<ACharacter>					Owner;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;
    
	UPROPERTY(BlueprintReadOnly, Category = "MovementState")
	TObjectPtr<UHDMovementStateComponent>   MovementState;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UHDCombatComponent>          Combat;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector									Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float									GroundSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float									MovingThreshould;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EHDCharacterStanceState				    CharacterStanceState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EHDCharacterMovementState				CharacterMovementState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint8									bIsIdle : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint8									bIsFalling : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint8									bIsJumping : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float									JumpingThreshould;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint8									bIsRotateRootBone : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float									YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float									Lean;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint8									bIsLookingViewport : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EHDCombatState							CombatState;
		
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	EHDTurningInPlace						TurningInPlace;

	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	FTransform								LeftHandTransform;
	
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	FRotator								RightHandRotation;
	
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	uint8									bIsShouldering : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	uint8									bUseAimOffset : 1 = true;
		
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	uint8									bUseFABRIK : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Aim")
    uint8									bTransformRightHand : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	float									AimOffset_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	float									AimOffset_Pitch;	
	
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	FVector									HitTarget;	
		
	UPROPERTY(BlueprintReadOnly, Category = "Throw")
	uint8									bIsUpperSlotValid : 1;
	
	FRotator								CharacterRotationLastFrame;
	FRotator								CharacterRotation;
	FRotator								DeltaRotation;
};
