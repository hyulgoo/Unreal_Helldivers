// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/HDGASPlayerState.h"
#include "Component/HDAbilitySystemComponent.h"

AHDGASPlayerState::AHDGASPlayerState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AbilitySystemComponent = CreateDefaultSubobject<UHDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* AHDGASPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
