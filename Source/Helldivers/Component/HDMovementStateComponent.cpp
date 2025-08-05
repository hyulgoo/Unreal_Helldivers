// Fill out your copyright notice in the Description page of Project Settings.

#include "HDMovementStateComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Define/HDDefine.h"

#define CHARACTERMOVESTATEZOFFSET 40.f

const EHDCharacterStanceState UHDMovementStateComponent::GetStanceState() const
{
	return StanceState;
}

void UHDMovementStateComponent::SetStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
	CONDITION_CHECK(NewState != EHDCharacterStanceState::Count);

	PrevStanceState = (PrevStanceState != StanceState) ? StanceState : PrevStanceState;
	StanceState = NewState;

	ChangeCameraZOffsetByCharacterMovementState(StanceState);
}

void UHDMovementStateComponent::RestoreStanceState()
{
	if (StanceState == EHDCharacterStanceState::Crouch)
	{
		StanceState = EHDCharacterStanceState::Idle;
	}
	else if (StanceState == EHDCharacterStanceState::Prone)
	{
		StanceState = PrevStanceState == EHDCharacterStanceState::Crouch ? PrevStanceState : EHDCharacterStanceState::Idle;
	}
	else
	{
		CONDITION_CHECK(false);
	}

	ChangeCameraZOffsetByCharacterMovementState(StanceState);
}

const EHDCharacterMovementState UHDMovementStateComponent::GetMovementState() const
{
    return MovementState;
}

void UHDMovementStateComponent::SetMovementState(const EHDCharacterMovementState NewState)
{
    CONDITION_CHECK(NewState != EHDCharacterMovementState::Count);

    MovementState = NewState;
}

void UHDMovementStateComponent::SetSpringArmDefaultZOffset(const float ZOffset)
{
	SpringArmZOffset = ZOffset;
}

void UHDMovementStateComponent::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State)
{
	USpringArmComponent* SpringArm = GetOwner()->GetComponentByClass<USpringArmComponent>();
	switch (State)
	{
	case EHDCharacterStanceState::Idle:
		SpringArm->TargetOffset.Z = SpringArmZOffset;
		break;
	case EHDCharacterStanceState::Crouch:
		SpringArm->TargetOffset.Z = SpringArmZOffset - CHARACTERMOVESTATEZOFFSET;
		break;
	case EHDCharacterStanceState::Prone:
		SpringArm->TargetOffset.Z = SpringArmZOffset - CHARACTERMOVESTATEZOFFSET * 2;
		break;
	default:
		LOG(TEXT("EHDCharacterMovementState is Invalid!!"));
		break;
	}
}