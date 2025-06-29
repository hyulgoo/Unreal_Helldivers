// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/HDAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Interface/HDWeaponInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Weapon/HDWeapon.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"

UHDAnimInstance::UHDAnimInstance()
	: Owner(nullptr)
	, Movement(nullptr)
	, Velocity(FVector())
	, GroundSpeed(0.f)
	, MovingThreshould(0.f)
	, CharacterMoveState(EHDCharacterMovementState::Idle)
	, bIsIdle(false)
	, bIsFalling(false)
	, bIsJumping(false)
	, JumpingThreshould(0.f)
	, bIsRotateRootBone(false)
	, bIsSprint(false)
	, YawOffset(0.f)
	, Lean(0.f)
	, bIsLookingViewport(false)
	, CombatState(EHDCombatState::Count)
	, TurningInPlace(EHDTurningInPlace::NotTurning)
	, LeftHandTransform(FTransform())
	, RightHandRotation(FRotator())
	, bIsShouldering(false)
	, bUseAimOffset(false)
	, bUseFABRIK(false)
	, bTransformRightHand(false)
	, AimOffset_Yaw(0.f)
	, AimOffset_Pitch(0.f)
	, HitTarget(FVector())
	, bIsUpperSlotValid(false)
	, CharacterRotationLastFrame(FRotator())
	, CharacterRotation(FRotator())
	, DeltaRotation(FRotator())
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void UHDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	NULL_CHECK(Owner);

	Movement = Owner->GetCharacterMovement();
	NULL_CHECK(Movement);
}

void UHDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	NULL_CHECK(Movement);

	// Character Input State
	Velocity = Movement->Velocity;
	GroundSpeed = Velocity.Size2D();
	bIsIdle = GroundSpeed < MovingThreshould;
	bIsFalling = Movement->IsFalling();
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

	// MovementInterface
	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = Owner;
	if (CharacterMovementInterface)
	{
		bIsSprint			= CharacterMovementInterface->IsSprint();
		bIsShouldering		= CharacterMovementInterface->IsShouldering();
		AimOffset_Yaw		= bIsShouldering ? 0.f : CharacterMovementInterface->GetAimOffset_Yaw();
		AimOffset_Pitch		= CharacterMovementInterface->GetAimOffset_Pitch();
		bIsRotateRootBone	= CharacterMovementInterface->IsUseRotateBone();
		TurningInPlace		= CharacterMovementInterface->GetTurningInPlace();
		bIsLookingViewport	= CharacterMovementInterface->IsCharacterLookingViewport();
		YawOffset			= bIsLookingViewport ? DeltaRotation.Yaw : 0.f;
		CharacterMoveState	= CharacterMovementInterface->GetCharacterMovementState();
	}

	// WeaponInterface
	USkeletalMeshComponent* CharacterMesh = Owner->GetMesh();
	TScriptInterface<IHDWeaponInterface> WeaponInterface = Owner;
	VALID_CHECK(CharacterMesh);
	NULL_CHECK(WeaponInterface);

	CombatState = WeaponInterface->GetCombatState();
	AHDWeapon* Weapon = WeaponInterface->GetWeapon();
	if (CombatState != EHDCombatState::Unoccupied && CombatState != EHDCombatState::Reloading)
	{
		bUseFABRIK = false;
		bTransformRightHand = false;
		if (Weapon)
		{
			HitTarget = WeaponInterface->GetHitTarget();
		}
	}
	else
	{
		if (Weapon)
		{
			USkeletalMeshComponent* WeaponMesh = Weapon->GetWeaponMesh();
			NULL_CHECK(WeaponMesh);

			LeftHandTransform = WeaponMesh->GetSocketTransform(HDSOCKETNAME_LEFTHAND, ERelativeTransformSpace::RTS_World);
			HitTarget = WeaponInterface->GetHitTarget();
			FVector OutPosition;
			FRotator OutRotation;
			CharacterMesh->TransformToBoneSpace(HDBONENAME_RIGHTHAND, LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			const FTransform& MuzzleFlashSocketTransform = WeaponMesh->GetSocketTransform(HDSOCKETNAME_MUZZLEFLASH, ERelativeTransformSpace::RTS_World);
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleFlashSocketTransform.GetLocation(), HitTarget);
			RightHandRotation = LookAtRotation;

			bUseFABRIK = CombatState == EHDCombatState::Unoccupied;
			bTransformRightHand = CombatState == EHDCombatState::ThrowingGrenade;
		}		
	}
}
