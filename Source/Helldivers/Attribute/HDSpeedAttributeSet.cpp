// Fill out your copyright notice in the Description page of Project Settings.

#include "HDSpeedAttributeSet.h"

UHDSpeedAttributeSet::UHDSpeedAttributeSet()
	: CurrentSpeed(FGameplayAttributeData())
	, CrawlingSpeed(FGameplayAttributeData())
	, CrouchSpeed(FGameplayAttributeData())
	, WalkSpeed(FGameplayAttributeData())
	, SprintSpeed(FGameplayAttributeData())
	, CurrentStamina(FGameplayAttributeData())
	, MaxStamina(FGameplayAttributeData())
{
}

void UHDSpeedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = NewValue < 0.f ? 0.f : NewValue;
}
