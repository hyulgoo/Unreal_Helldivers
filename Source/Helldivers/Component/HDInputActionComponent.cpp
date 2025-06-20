// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/HDInputActionComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameFramework/SpringArmComponent.h"
#include "Define/HDDefine.h"

#define CHARACTERMOVESTATEZOFFSET 40.f

UHDInputActionComponent::UHDInputActionComponent()
	: MovementState(EHDCharacterMovementState::Idle)
	, PrevMovementState(EHDCharacterMovementState::Idle)
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

const EHDCharacterMovementState UHDInputActionComponent::GetCharacterMovementState() const
{
	return MovementState;
}

void UHDInputActionComponent::SetCharacterMovementState(const EHDCharacterMovementState NewState, const bool bForced)
{
	CONDITION_CHECK(NewState == EHDCharacterMovementState::Count);

	PrevMovementState = (PrevMovementState != MovementState) ? MovementState : PrevMovementState;
	MovementState = NewState;

	ChangeCameraZOffsetByCharacterMovementState(MovementState);
}

void UHDInputActionComponent::RestoreMovementState()
{
	if (MovementState == EHDCharacterMovementState::Crouch)
	{
		MovementState = EHDCharacterMovementState::Idle;
	}
	else if (MovementState == EHDCharacterMovementState::Prone)
	{
		MovementState = PrevMovementState == EHDCharacterMovementState::Crouch ? PrevMovementState : EHDCharacterMovementState::Idle;
	}
	else
	{
		CONDITION_CHECK(true);
	}

	ChangeCameraZOffsetByCharacterMovementState(MovementState);
}

void UHDInputActionComponent::SetSpringArmDefaultZOffset(const float ZOffset)
{
	SpringArmZOffset = ZOffset;
}

void UHDInputActionComponent::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterMovementState State)
{
	USpringArmComponent* SpringArm = GetOwner()->GetComponentByClass<USpringArmComponent>();
	switch (State)
	{
	case EHDCharacterMovementState::Idle:
		SpringArm->TargetOffset.Z = SpringArmZOffset;
		break;
	case EHDCharacterMovementState::Crouch:
		SpringArm->TargetOffset.Z = SpringArmZOffset - CHARACTERMOVESTATEZOFFSET;
		break;
	case EHDCharacterMovementState::Prone:
		SpringArm->TargetOffset.Z = SpringArmZOffset - CHARACTERMOVESTATEZOFFSET * 2;
		break;
	default:
		LOG(TEXT("EHDCharacterMovementState is Invalid!!"));
		break;
	}
}