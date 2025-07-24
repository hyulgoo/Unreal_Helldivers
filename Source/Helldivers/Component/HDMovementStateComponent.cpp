// Fill out your copyright notice in the Description page of Project Settings.

#include "HDMovementStateComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameData/HDCharacterControlData.h"
#include "Define/HDDefine.h"

#define CHARACTERMOVESTATEZOFFSET 40.f

UHMovementStateComponent::UHMovementStateComponent()
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

const EHDCharacterStanceState UHMovementStateComponent::GetStanceState() const
{
	return StanceState;
}

void UHMovementStateComponent::SetStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
	CONDITION_CHECK(NewState != EHDCharacterStanceState::Count);

	PrevStanceState = (PrevStanceState != StanceState) ? StanceState : PrevStanceState;
	StanceState = NewState;

	ChangeCameraZOffsetByCharacterMovementState(StanceState);
}

void UHMovementStateComponent::RestoreStanceState()
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

const EHDCharacterMovementState UHMovementStateComponent::GetMovementState() const
{
    return MovementState;
}

void UHMovementStateComponent::SetMovementState(const EHDCharacterMovementState NewState)
{
    CONDITION_CHECK(NewState != EHDCharacterMovementState::Count);

    MovementState = NewState;
}

void UHMovementStateComponent::SetSpringArmDefaultZOffset(const float ZOffset)
{
	SpringArmZOffset = ZOffset;
}

void UHMovementStateComponent::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State)
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

UHDCharacterControlData* UHMovementStateComponent::SetControlType(const EHDCharacterControlType Type)
{
    CONDITION_CHECK_WITH_RETURNTYPE(CharacterControlDataMap.IsEmpty() == false, nullptr);

    CurrentCharacterControlType = Type;
    return CharacterControlDataMap[CurrentCharacterControlType];
}

const EHDCharacterControlType UHMovementStateComponent::GetControlType() const
{
    return CurrentCharacterControlType;
}
