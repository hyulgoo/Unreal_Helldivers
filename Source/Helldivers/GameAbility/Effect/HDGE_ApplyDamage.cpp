// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGE_ApplyDamage.h"
#include "Tag/HDGameplayTag.h"
#include "Attribute/HDBaseAttributeSet.h"

UHDGE_ApplyDamage::UHDGE_ApplyDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo HealthModifier;
    HealthModifier.Attribute = FGameplayAttribute((UHDBaseAttributeSet::GetCurrentHealthAttribute()));
    HealthModifier.ModifierOp = EGameplayModOp::Additive;

    FSetByCallerFloat CallerData;
    CallerData.DataName = FName(TEXT("Data.Projectile.Damage"));
    CallerData.DataTag = HDTAG_DATA_PROJECTILE_DAMAGE;
    
    HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CallerData);
    Modifiers.Add(HealthModifier);
}
