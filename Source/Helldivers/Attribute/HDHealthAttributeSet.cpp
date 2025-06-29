// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/HDHealthAttributeSet.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Character/HDCharacterBase.h"
#include "GameplayEffectExtension.h"
#include "Tag/HDGameplayTag.h"

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
	return true;
}

void UHDHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC)
		{
			ASC->AddLooseGameplayTag(HDTAG_CHARACTER_STATE_ISDEAD);
		}
	}
}
