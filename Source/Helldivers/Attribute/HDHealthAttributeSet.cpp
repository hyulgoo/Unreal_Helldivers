// Fill out your copyright notice in the Description page of Project Settings.

#include "HDHealthAttributeSet.h"
#include "Define/HDGameplayTag.h"
#include "Define/HDDefine.h"
#include "AbilitySystem/GameplayAbilityHelper.h"

UHDHealthAttributeSet::UHDHealthAttributeSet()
	: CurrentHealth(FGameplayAttributeData())
	, MaxHealth(FGameplayAttributeData())
{
}

void UHDHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    if(Attribute == GetCurrentHealthAttribute()) 
    {
        NewValue = NewValue <= 0.f ? 0.f : (NewValue > GetMaxHealth() ? GetMaxHealth() : NewValue);
    }
}

void UHDHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetCurrentHealthAttribute())
	{
		if (OldValue > 0.f && NewValue <= 0.f)
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			NULL_CHECK(ASC);

			FGameplayAbilityHelper::AddTagToTarget(ASC, HDTAG_CHARACTER_STATE_ISDEAD);
		}
	}
}
