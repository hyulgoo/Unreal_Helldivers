// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "HDMovementStateComponent.generated.h"

UCLASS()
class HELLDIVERS_API UHDMovementStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	const EHDCharacterStanceState	GetStanceState() const;
	void							SetStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	void							RestoreStanceState();

    const EHDCharacterMovementState GetMovementState() const;
    void                            SetMovementState(const EHDCharacterMovementState NewState);

	void							SetSpringArmDefaultZOffset(const float ZOffset);
	void							ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State);

private:
	EHDCharacterStanceState		    StanceState         = EHDCharacterStanceState::Idle;
	EHDCharacterStanceState		    PrevStanceState     = EHDCharacterStanceState::Idle;
	EHDCharacterMovementState       MovementState       = EHDCharacterMovementState::Idle;
	
	float					        SpringArmZOffset    = 0.f;	
};
