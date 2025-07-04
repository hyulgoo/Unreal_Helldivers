// Fill out your copyright notice in the Description page of Project Settings.

#include "HDSpeedAttributeSet.h"
#include "Define/HDDefine.h"

UHDSpeedAttributeSet::UHDSpeedAttributeSet()
	: CurrentSpeed(FGameplayAttributeData())
	, CrawlingSpeed(FGameplayAttributeData())
	, CrouchSpeed(FGameplayAttributeData())
	, WalkSpeed(FGameplayAttributeData())
	, SprintSpeed(FGameplayAttributeData())
	, CurrentStamina(FGameplayAttributeData())
	, MaxStamina(FGameplayAttributeData())
{
	InitCurrentSpeed(GetWalkSpeed());
}

void UHDSpeedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = NewValue < 0.f ? 0.f : NewValue;
}

bool UHDSpeedAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return true;
}

void UHDSpeedAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
