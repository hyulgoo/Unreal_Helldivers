// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGE_ApplyDamage.h"
#include "Tag/HDGameplayTag.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "Attribute/HDHealthAttributeSet.h"

UHDGE_ApplyDamage::UHDGE_ApplyDamage(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    UTargetTagRequirementsGameplayEffectComponent* TagReqComp =
        ObjectInitializer.CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(this, TEXT("TargetTagReq"));

    const FGameplayTag DeadTag = HDTAG_CHARACTER_STATE_ISDEAD;
    TagReqComp->ApplicationTagRequirements.IgnoreTags.AddTag(DeadTag);

    FGameplayModifierInfo HealthModifier;
    HealthModifier.Attribute = UHDHealthAttributeSet::GetCurrentHealthAttribute();
    HealthModifier.ModifierOp = EGameplayModOp::Additive;

    FSetByCallerFloat CallerData;
    CallerData.DataName = FName(TEXT("Data.Damage.Projectile"));
    CallerData.DataTag = HDTAG_DATA_DAMAGE_PROJECTILE;
    
    HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(CallerData);
    Modifiers.Add(HealthModifier);
}
