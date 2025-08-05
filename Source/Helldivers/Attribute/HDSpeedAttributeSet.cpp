// Fill out your copyright notice in the Description page of Project Settings.

#include "HDSpeedAttributeSet.h"

void UHDSpeedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = NewValue < 0.f ? 0.f : NewValue;
}
