// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/HDCharacterControlData.h"

UHDCharacterControlData::UHDCharacterControlData()
	: bUseControllerRotationYaw(false)
	, bOrientRotationToMovement(false)
	, bUseControllerDesiredRotation(false)
	, RotationRate(FRotator())
	, InputMappingContext(nullptr)
	, TargetArmLength(0.f)
	, TargetOffset(FVector())
	, SocketOffset(FVector())
    , bUsePawnControlRotation(false)
    , bInheritPitch(false)
    , bInheritYaw(false)
    , bInheritRoll(false)
    , bDoCollisionTest(false)
{
	TargetArmLength = 400.f;
}
