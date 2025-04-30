// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_ApplyDamage.h"
#include "Attribute/HDCharacterAttributeSet.h"

UHDGE_ApplyDamage::UHDGE_ApplyDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo HealthModifier;
    HealthModifier.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(UHDCharacterAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UHDCharacterAttributeSet, CurrentHealth)));

}
