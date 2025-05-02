// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGE_ApplyDamage.h"
#include "Attribute/HDBaseAttributeSet.h"

UHDGE_ApplyDamage::UHDGE_ApplyDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo HealthModifier;
    HealthModifier.Attribute = FGameplayAttribute((UHDBaseAttributeSet::GetCurrentHealthAttribute()));
    HealthModifier.ModifierOp = EGameplayModOp::Additive;
    
    FScalableFloat DamageAmount(-30.0f);
    FGameplayEffectModifierMagnitude ModMagnitude(DamageAmount);
    
    HealthModifier.ModifierMagnitude = ModMagnitude;
    Modifiers.Add(HealthModifier);
}
