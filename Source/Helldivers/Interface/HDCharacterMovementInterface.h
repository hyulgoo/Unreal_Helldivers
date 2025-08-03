// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDCharacterMovementInterface.generated.h"

enum class EHDTurningInPlace : uint8;
enum class EHDCharacterMovementState : uint8;
enum class EHDCharacterStanceState : uint8;
enum class EHDCharacterControlType : uint8;
enum class EHDCombatState : uint8;

UINTERFACE(MinimalAPI)
class UHDCharacterMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDCharacterMovementInterface
{
	GENERATED_BODY()

public:
	virtual void SetShouldering(const bool bSetAiming) = 0;
	virtual void SetMovementState(const EHDCharacterMovementState NewState) = 0;
	virtual void SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced = false) = 0;
	virtual void RestoreStanceState() = 0;
};
