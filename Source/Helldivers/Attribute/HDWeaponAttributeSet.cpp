// Fill out your copyright notice in the Description page of Project Settings.

#include "HDWeaponAttributeSet.h"
#include "Define/HDGameplayTag.h"
#include "AbilitySystemComponent.h"

void UHDWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    if(Attribute == GetAmmoAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAmmo());
    }
    else if (Attribute == GetCapacityAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxCapacity());
    }
    else if (Attribute == GetGrenadeAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxGrenade());
    }
}

void UHDWeaponAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    if (OldValue > 0.f && NewValue <= 0.f)
    {
        if (Attribute == GetAmmoAttribute())
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(Tag_Block_Fire);
        }
        else if (Attribute == GetCapacityAttribute())
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(Tag_Block_Reload);
        }
        else if (Attribute == GetGrenadeAttribute())
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(Tag_Block_Grenade);
        }
    }
    else if (OldValue <= 0.f && NewValue > 0.f)
    {
        if (Attribute == GetAmmoAttribute())
        {
            GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(Tag_Block_Fire);
        }
        else if (Attribute == GetCapacityAttribute())
        {
            GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(Tag_Block_Reload);
        }
        else if (Attribute == GetGrenadeAttribute())
        {
            GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(Tag_Block_Grenade);
        }
        }
}
