// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"
#include "Define/HDDefine.h"

UHDPlayerSpeedAttributeSet::UHDPlayerSpeedAttributeSet()
	: CurrentSpeed(FGameplayAttributeData())
	, CrawlingSpeed(FGameplayAttributeData())
	, CrouchSpeed(FGameplayAttributeData())
	, WalkSpeed(FGameplayAttributeData())
	, SprintSpeed(FGameplayAttributeData())
	, MaxStamina(FGameplayAttributeData())
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
