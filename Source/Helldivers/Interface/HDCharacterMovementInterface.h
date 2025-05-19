// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDCharacterMovementInterface.generated.h"

enum class EHDTurningInPlace : uint8;
enum class ECharacterMovementMode : uint8;
enum class EHDCharacterControlType : uint8;

UINTERFACE(MinimalAPI)
class UHDCharacterMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDCharacterMovementInterface
{
	GENERATED_BODY()

public:
	// Aiming
	virtual const float						GetAimOffset_Yaw() const = 0;
	virtual const float						GetAimOffset_Pitch() const = 0;

	// Shouldering
	virtual const bool						IsShouldering() const = 0;
	virtual void							SetShouldering(const bool bSetAiming) = 0;
	virtual const bool						IsCharacterLookingViewport() const = 0;

	// CharacterControl
	virtual const EHDCharacterControlType	GetCharacterControlType() const = 0;
	virtual void							ChangeCharacterControlType() = 0;

	// Turn In Place
	virtual const EHDTurningInPlace			GetTurningInPlace() const = 0;
	virtual const bool						IsUseRotateBone() const = 0;

	virtual const bool						IsSprint() const = 0;
	virtual void							SetSprint(const bool bSprint) = 0;

};
