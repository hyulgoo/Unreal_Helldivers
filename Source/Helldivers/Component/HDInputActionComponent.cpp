// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/HDInputActionComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameFramework/SpringArmComponent.h"
#include "Define/HDDefine.h"

#define CHARACTERMOVESTATEZOFFSET 40.f

UHDInputActionComponent::UHDInputActionComponent()
	: PoseState(EHDCharacterPoseState::Idle)
	, PrevPoseState(EHDCharacterPoseState::Idle)
	, bIsSprint(false)
	, SpringArmZOffset(0.f)
{
}

const bool UHDInputActionComponent::IsSprint() const
{
	return bIsSprint;
}

void UHDInputActionComponent::SetSprint(const bool bSprint)
{
	bIsSprint = bSprint;
}

const EHDCharacterPoseState UHDInputActionComponent::GetCharacterPoseState() const
{
	return PoseState;
}

void UHDInputActionComponent::SetCharacterPoseState(const EHDCharacterPoseState NewState, const bool bForced)
{
	CONDITION_CHECK(NewState == EHDCharacterPoseState::Count);

	PrevPoseState = (PrevPoseState != PoseState) ? PoseState : PrevPoseState;
	PoseState = NewState;

	ChangeCameraZOffsetByCharacterMovementState(PoseState);
}

void UHDInputActionComponent::RestorePoseState()
{
	if (PoseState == EHDCharacterPoseState::Crouch)
	{
		PoseState = EHDCharacterPoseState::Idle;
	}
	else if (PoseState == EHDCharacterPoseState::Prone)
	{
		PoseState = PrevPoseState == EHDCharacterPoseState::Crouch ? PrevPoseState : EHDCharacterPoseState::Idle;
	}
	else
	{
		CONDITION_CHECK(true);
	}

	ChangeCameraZOffsetByCharacterMovementState(PoseState);
}

const EHDCharacterMovementState UHDInputActionComponent::GetCharacterMovementState() const
{
    return MovementState;
}

void UHDInputActionComponent::SetCharacterMovementState(const EHDCharacterMovementState NewState)
{
    MovementState = NewState;
}

void UHDInputActionComponent::SetSpringArmDefaultZOffset(const float ZOffset)
{
	SpringArmZOffset = ZOffset;
}

void UHDInputActionComponent::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterPoseState State)
{
	USpringArmComponent* SpringArm = GetOwner()->GetComponentByClass<USpringArmComponent>();
	switch (State)
	{
	case EHDCharacterPoseState::Idle:
		SpringArm->TargetOffset.Z = SpringArmZOffset;
		break;
	case EHDCharacterPoseState::Crouch:
		SpringArm->TargetOffset.Z = SpringArmZOffset - CHARACTERMOVESTATEZOFFSET;
		break;
	case EHDCharacterPoseState::Prone:
		SpringArm->TargetOffset.Z = SpringArmZOffset - CHARACTERMOVESTATEZOFFSET * 2;
		break;
	default:
		LOG(TEXT("EHDCharacterMovementState is Invalid!!"));
		break;
	}
}