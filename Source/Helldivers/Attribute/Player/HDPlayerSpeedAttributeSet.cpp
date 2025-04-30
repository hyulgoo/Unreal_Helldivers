// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/HDPlayerSpeedAttributeSet.h"

UHDPlayerSpeedAttributeSet::UHDPlayerSpeedAttributeSet()
    : CurrentSpeed(0.f)
    , CrawlingSpeed(0.f)
    , CrouchSpeed(0.f)
    , WalkSpeed(0.f)
    , SprintSpeed(0.f)
{
	InitCurrentSpeed(GetWalkSpeed());
}

void UHDPlayerSpeedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

bool UHDPlayerSpeedAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
	{
		return false;
	}

	return true;
}

void UHDPlayerSpeedAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
