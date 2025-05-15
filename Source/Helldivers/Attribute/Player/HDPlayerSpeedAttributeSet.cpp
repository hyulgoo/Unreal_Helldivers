// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"

UHDPlayerSpeedAttributeSet::UHDPlayerSpeedAttributeSet()
{
	InitCurrentSpeed(GetWalkSpeed());
}

void UHDPlayerSpeedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = NewValue < 0.f ? 0.f : NewValue;
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
