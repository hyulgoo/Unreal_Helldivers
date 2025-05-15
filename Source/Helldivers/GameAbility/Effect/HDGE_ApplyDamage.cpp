// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGE_ApplyDamage.h"
#include "Tag/HDGameplayTag.h"
#include "Attribute/HDHealthAttributeSet.h"

UHDGE_ApplyDamage::UHDGE_ApplyDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo HealthModifier;
    HealthModifier.Attribute = FGameplayAttribute((UHDHealthAttributeSet::GetCurrentHealthAttribute()));
    HealthModifier.ModifierOp = EGameplayModOp::Additive;

    FSetByCallerFloat CallerData;
    CallerData.DataName = FName(TEXT("Data.Damage.Projectile"));
    CallerData.DataTag = HDTAG_DATA_DAMAGE_PROJECTILE;
    
    HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CallerData);
    Modifiers.Add(HealthModifier);
}
