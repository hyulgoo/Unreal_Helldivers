// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/HDCharacterAttributeSet.h"

UHDCharacterAttributeSet::UHDCharacterAttributeSet()
	: Health(0.f)
	, MaxHealth(100.f)
	, Speed(100.f)
{
	InitHealth(GetMaxHealth());
}

void UHDCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

bool UHDCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
	{
		return false;
	}

	return true;
}

void UHDCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
