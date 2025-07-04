// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/HDInputActionComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameFramework/SpringArmComponent.h"
#include "Define/HDDefine.h"

#define CHARACTERMOVESTATEZOFFSET 40.f

UHDInputActionComponent::UHDInputActionComponent()
	: StanceState(EHDCharacterStanceState::Idle)
	, PrevStanceState(EHDCharacterStanceState::Idle)
    , MovementState(EHDCharacterMovementState::Idle)
	, SpringArmZOffset(0.f)
    , CurrentCharacterControlType(EHDCharacterControlType::ThirdPerson)
    , CharacterControlDataMap{}
{
    static ConstructorHelpers::FObjectFinder<UHDCharacterControlData> ThirdPersonDataRef(TEXT("/Script/Helldivers.HDCharacterControlData'/Game/Helldivers/CharacterControl/HDC_ThirdPerson.HDC_ThirdPerson'"));
    if (ThirdPersonDataRef.Succeeded())
    {
        CharacterControlDataMap.Add(EHDCharacterControlType::ThirdPerson, ThirdPersonDataRef.Object);
    }

    static ConstructorHelpers::FObjectFinder<UHDCharacterControlData> FirstPersonDataRef(TEXT("/Script/Helldivers.HDCharacterControlData'/Game/Helldivers/CharacterControl/HDC_FirstPerson.HDC_FirstPerson'"));
    if (FirstPersonDataRef.Succeeded())
    {
        CharacterControlDataMap.Add(EHDCharacterControlType::FirstPerson, FirstPersonDataRef.Object);
    }
}

const EHDCharacterStanceState UHDInputActionComponent::GetStanceState() const
{
	return StanceState;
}

void UHDInputActionComponent::SetStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
	CONDITION_CHECK(NewState == EHDCharacterStanceState::Count);

	PrevStanceState = (PrevStanceState != StanceState) ? StanceState : PrevStanceState;
	StanceState = NewState;

	ChangeCameraZOffsetByCharacterMovementState(StanceState);
}

void UHDInputActionComponent::RestoreStanceState()
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
		CONDITION_CHECK(true);
	}

	ChangeCameraZOffsetByCharacterMovementState(StanceState);
}

const EHDCharacterMovementState UHDInputActionComponent::GetMovementState() const
{
    return MovementState;
}

void UHDInputActionComponent::SetMovementState(const EHDCharacterMovementState NewState)
{
    CONDITION_CHECK(NewState == EHDCharacterMovementState::Count);

    MovementState = NewState;
}

void UHDInputActionComponent::SetSpringArmDefaultZOffset(const float ZOffset)
{
	SpringArmZOffset = ZOffset;
}

void UHDInputActionComponent::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State)
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

UHDCharacterControlData* UHDInputActionComponent::SetControlType(const EHDCharacterControlType Type)
{
    CONDITION_CHECK_WITH_RETURNTYPE(CharacterControlDataMap.IsEmpty(), nullptr);

    CurrentCharacterControlType = Type;
    return CharacterControlDataMap[CurrentCharacterControlType];
}

const EHDCharacterControlType UHDInputActionComponent::GetControlType() const
{
    return CurrentCharacterControlType;
}

const TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>>& UHDInputActionComponent::GetInputActionMap() const
{
    return InputActionMap;
}
