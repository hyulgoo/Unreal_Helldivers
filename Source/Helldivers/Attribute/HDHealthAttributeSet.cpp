// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/HDHealthAttributeSet.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

UHDHealthAttributeSet::UHDHealthAttributeSet()
	: CurrentHealth(FGameplayAttributeData())
	, MaxHealth(FGameplayAttributeData())
{
	InitCurrentHealth(GetMaxHealth());
}

void UHDHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    if(Attribute == GetCurrentHealthAttribute()) 
    {
        NewValue = NewValue <= 0.f ? 0.f : (NewValue > GetMaxHealth() ? GetMaxHealth() : NewValue);
    }	
}

bool UHDHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
	{
		return false;
	}

	return true;
}

void UHDHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
