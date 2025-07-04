// Fill out your copyright notice in the Description page of Project Settings.

#include "HDAbilitySystemComponent.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "GameData/HDCharacterStat.h"
#include "Define/HDDefine.h"
#include "Define/HDGameplayTag.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

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
	UHDHealthAttributeSet* HealthAttributeSet = NewObject<UHDHealthAttributeSet>(this);
	UHDSpeedAttributeSet* ApeedAttributeSet = NewObject<UHDSpeedAttributeSet>(this);
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

    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXHEALTH,       StatData->MaxHealth);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_WALKSPEED,       StatData->WalkSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_CRAWLINGSPEED,   StatData->CrawlingSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_CROUCHSPEED,     StatData->CrouchSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_SPRINTSPEED,     StatData->SprintSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXSTAMINA,      StatData->MaxStamina);

    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
