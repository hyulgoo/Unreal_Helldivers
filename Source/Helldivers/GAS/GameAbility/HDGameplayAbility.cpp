// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGameplayAbility.h"

UHDGameplayAbility::UHDGameplayAbility()
    : AbilityInputTags(FGameplayTagContainer())
{
}

const FGameplayTagContainer& UHDGameplayAbility::GetAssetInputTags() const
{
    return AbilityInputTags;
}
