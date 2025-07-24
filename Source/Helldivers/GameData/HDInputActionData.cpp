#pragma once

#include "HDInputActionData.h"

FHDInputAction::FHDInputAction()
    : InputAction(nullptr)
    , InputTag(FGameplayTag())
    , TriggerType(EHDTriggerType::None)
{

}

UHDInputData::UHDInputData()
    : AbilityInputActions{}
{
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
