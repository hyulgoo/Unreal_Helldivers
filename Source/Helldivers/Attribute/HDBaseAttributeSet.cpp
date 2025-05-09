// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/HDBaseAttributeSet.h"

void UHDBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

bool UHDBaseAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    return false;
}

void UHDBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
}
