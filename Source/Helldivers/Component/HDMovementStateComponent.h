// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDMovementStateComponent.generated.h"

enum class EHDCharacterStanceState : uint8;
enum class EHDCharacterMovementState : uint8;

UCLASS()
class HELLDIVERS_API UHMovementStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	explicit						UHMovementStateComponent();

	const EHDCharacterStanceState	GetStanceState() const;
	void							SetStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	void							RestoreStanceState();

    const EHDCharacterMovementState GetMovementState() const;
    void                            SetMovementState(const EHDCharacterMovementState NewState);

	void							SetSpringArmDefaultZOffset(const float ZOffset);
	void							ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State);

private:
	EHDCharacterStanceState		    StanceState;
	EHDCharacterStanceState		    PrevStanceState;
	EHDCharacterMovementState       MovementState;
	
	float					        SpringArmZOffset;	
};
