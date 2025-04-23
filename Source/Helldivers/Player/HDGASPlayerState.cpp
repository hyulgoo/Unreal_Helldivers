// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HDGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/HDCharacterAttributeSet.h"

AHDGASPlayerState::AHDGASPlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSet = CreateDefaultSubobject<UHDCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

UAbilitySystemComponent* AHDGASPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
