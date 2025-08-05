#pragma once

#include "HDInputActionData.h"
#include "Define/HDGameplayTag.h"
#include "AbilitySystem/GameplayAbilityHelper.h"

FHDInputAction::FHDInputAction(UInputAction* NewInputAction, const FGameplayTag& NewInputTag, const FGameplayTag& NewAbilityTriggerTag, const EHDTriggerType  NewTriggerType)
    : InputAction(NewInputAction)
    , InputTag(NewInputTag)
    , AbilityTriggerTag(NewAbilityTriggerTag)
    , TriggerType(NewTriggerType)
{
}

UHDInputData::UHDInputData(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    const FGameplayTagContainer ChildTags = FGameplayAbilityHelper::GetAllChildTag(HDTAG_INPUT);
    for (const FGameplayTag& ChildTag : ChildTags)
    {
        AbilityInputActions.Add(FHDInputAction(nullptr, ChildTag, FGameplayTag::EmptyTag, EHDTriggerType::None));
    }
}

const FGameplayTag UHDInputData::GetInputTagByTriggerTag(const FGameplayTag& TriggerTag)
{
    FGameplayTag InputTag;

    for (const FHDInputAction& Data : AbilityInputActions)
    {
        if (Data.AbilityTriggerTag == TriggerTag)
        {
            InputTag = Data.InputTag;
            break;
        }
    }

    return InputTag;
}

const FGameplayTag UHDInputData::GetTriggerTagByInputTag(const FGameplayTag& InputTag)
{
    FGameplayTag TriggerTag;

    for (const FHDInputAction& Data : AbilityInputActions)
    {
        if (Data.InputTag == InputTag)
        {
            TriggerTag = Data.AbilityTriggerTag;
            break;
        }
    }

    return TriggerTag;
}
