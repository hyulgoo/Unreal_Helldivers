// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Component/HDCombatComponent.h"
#include "Component/HDMovementStateComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"

UHDAnimInstance::UHDAnimInstance()
	: Owner(nullptr)
	, CharacterMovement(nullptr)
    , MovementState(nullptr)
    , Combat(nullptr)
	, Velocity(FVector::ZeroVector)
	, GroundSpeed(0.f)
	, MovingThreshould(0.f)
	, CharacterStanceState(EHDCharacterStanceState::Idle)
	, CharacterMovementState(EHDCharacterMovementState::Idle)
	, bIsIdle(false)
	, bIsFalling(false)
	, bIsJumping(false)
	, JumpingThreshould(0.f)
	, bIsRotateRootBone(false)
	, YawOffset(0.f)
	, Lean(0.f)
	, bIsLookingViewport(false)
	, CombatState(EHDCombatState::Count)
	, TurningInPlace(EHDTurningInPlace::NotTurning)
	, LeftHandTransform(FTransform::Identity)
	, RightHandRotation(FRotator::ZeroRotator)
	, bIsShouldering(false)
	, bUseAimOffset(false)
	, bUseFABRIK(false)
	, bTransformRightHand(false)
	, AimOffset_Yaw(0.f)
	, AimOffset_Pitch(0.f)
	, HitTarget(FVector::ZeroVector)
	, bIsUpperSlotValid(false)
	, CharacterRotationLastFrame(FRotator::ZeroRotator)
	, CharacterRotation(FRotator::ZeroRotator)
	, DeltaRotation(FRotator::ZeroRotator)
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void UHDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	NULL_CHECK(Owner);

	CharacterMovement = Owner->GetCharacterMovement();
	NULL_CHECK(CharacterMovement);

    Combat = Owner->GetComponentByClass<UHDCombatComponent>();
    NULL_CHECK(Combat);

    MovementState = Owner->GetComponentByClass<UHDMovementStateComponent>();
    NULL_CHECK(MovementState);
}

void UHDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	NULL_CHECK(CharacterMovement);

	// Character Input State
	Velocity = CharacterMovement->Velocity;
	GroundSpeed = Velocity.Size2D();
	bIsIdle = GroundSpeed < MovingThreshould;
	bIsFalling = CharacterMovement->IsFalling();
	bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);

	// Character Yaw for strafing
	const FRotator AimRotation = Owner->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Owner->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = Owner->GetActorRotation();
	DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = DeltaRot.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	// MovementState
    if (MovementState)
    {
        CharacterMovementState = MovementState->GetMovementState();
        CharacterStanceState = MovementState->GetStanceState();
        YawOffset = bIsLookingViewport ? DeltaRotation.Yaw : 0.f;
    }

    // Combat
    if (Combat)
    {
        USkeletalMeshComponent* CharacterMesh = Owner->GetMesh();
        VALID_CHECK(CharacterMesh);

        bIsShouldering      = Combat->IsShoulder();
        bIsRotateRootBone   = Combat->IsUseRotateBone();
        bIsLookingViewport  = Combat->IsCharacterLookingViewport();
        TurningInPlace      = Combat->GetTurnInPlace();
        AimOffset_Yaw       = bIsShouldering ? 0.f : Combat->GetAimOffset_Yaw();
        AimOffset_Pitch     = Combat->GetAimOffset_Pitch();
        CombatState         = Combat->GetCombatState();

        if (CombatState != EHDCombatState::Unoccupied && CombatState != EHDCombatState::Fire && CombatState != EHDCombatState::Reloading)
        {
            HitTarget = Combat->GetHitTarget();
        }
        else
        {
            USkeletalMeshComponent* WeaponMesh = Combat->GetWeaponMesh();
            NULL_CHECK(WeaponMesh);

            LeftHandTransform = WeaponMesh->GetSocketTransform(HDSOCKETNAME_LEFTHAND, ERelativeTransformSpace::RTS_World);
            HitTarget = Combat->GetHitTarget();
            FVector OutPosition;
            FRotator OutRotation;
            CharacterMesh->TransformToBoneSpace(HDBONENAME_RIGHTHAND, LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
            LeftHandTransform.SetLocation(OutPosition);
            LeftHandTransform.SetRotation(FQuat(OutRotation));

            //const FTransform& MuzzleFlashSocketTransform = WeaponMesh->GetSocketTransform(HDSOCKETNAME_MUZZLEFLASH, ERelativeTransformSpace::RTS_World);
            //const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleFlashSocketTransform.GetLocation(), HitTarget);
            //RightHandRotation = LookAtRotation;
        }

        bUseFABRIK = (CombatState == EHDCombatState::Unoccupied || CombatState == EHDCombatState::Fire);
        bIsUpperSlotValid = (CombatState == EHDCombatState::Fire || CombatState == EHDCombatState::Throwing);
    }
}
