// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HDInputActionComponent.generated.h"

enum class EHDCharacterMovementState : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLDIVERS_API UHDInputActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	explicit						UHDInputActionComponent();

	const bool						IsSprint() const ;
	void							SetSprint(const bool bSprint) ;

	const EHDCharacterMovementState	GetCharacterMovementState() const;
	void							SetCharacterMovementState(const EHDCharacterMovementState NewState, const bool bForced = false);
	void							RestoreMovementState();

	void							SetSpringArmDefaultZOffset(const float ZOffset);
	void							ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterMovementState State);

private:
	EHDCharacterMovementState		MovementState;
	EHDCharacterMovementState		PrevMovementState;
	bool							bIsSprint;
	
	float					        SpringArmZOffset;
};
