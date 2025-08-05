// Fill out your copyright notice in the Description page of Project Settings.

#include "HDTA_Trace.h"

void AHDTA_Trace::StartTargeting(UGameplayAbility* Ability)
{
}

void AHDTA_Trace::ConfirmTargetingAndContinue()
{
}

void AHDTA_Trace::SetShowDebug(const bool InShowDebug)
{
    bShowDebug = InShowDebug;
}

FGameplayAbilityTargetDataHandle AHDTA_Trace::MakeTargetData() const
{
    return FGameplayAbilityTargetDataHandle();
}
