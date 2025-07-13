// Fill out your copyright notice in the Description page of Project Settings.

#include "HDAbilitySystemComponent.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "GameData/HDCharacterStat.h"
#include "Define/HDDefine.h"
#include "Define/HDGameplayTag.h"

UHDAbilitySystemComponent::UHDAbilitySystemComponent()
{
}

void UHDAbilitySystemComponent::AbilityInputTagTriggered(const FGameplayTag Tag)
{
    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySpecInputPressed(Spec);
        }
        else
        {
            TryActivateAbility(Spec.Handle);
        }
    }
}

void UHDAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag Tag)
{
    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySpecInputReleased(Spec);
        }
    }
}

void UHDAbilitySystemComponent::AbilityInputTagToggled(const FGameplayTag Tag)
{
    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySpecInputReleased(Spec);
        }
        else
        {
            TryActivateAbility(Spec.Handle);
        }
    }
}

void UHDAbilitySystemComponent::InitAttributeSet()
{
	UHDHealthAttributeSet* HealthAttributeSet = NewObject<UHDHealthAttributeSet>(GetOwnerActor());
	UHDSpeedAttributeSet* ApeedAttributeSet = NewObject<UHDSpeedAttributeSet>(GetOwnerActor());
	AddAttributeSetSubobject(HealthAttributeSet);
	AddAttributeSetSubobject(ApeedAttributeSet);
}

void UHDAbilitySystemComponent::SetAttributeSetStat(FHDCharacterStat* StatData, TSubclassOf<UGameplayEffect> InitAttributeStatEffect)
{
    NULL_CHECK(StatData);
    NULL_CHECK(InitAttributeStatEffect);

    FGameplayEffectContextHandle Context = MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(InitAttributeStatEffect, 1.f, Context);
    CONDITION_CHECK(SpecHandle.IsValid() == false);

    for(const TPair<FGameplayTag, float>& TaggedStat : StatData->Stats)
    {
        SpecHandle.Data->SetSetByCallerMagnitude(TaggedStat.Key, TaggedStat.Value);
    }

    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
