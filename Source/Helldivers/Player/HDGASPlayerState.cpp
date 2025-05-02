// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/HDGASPlayerState.h"
#include "AbilitySystemComponent.h"

AHDGASPlayerState::AHDGASPlayerState()
{
    AbilitySystemComponent  = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* AHDGASPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
