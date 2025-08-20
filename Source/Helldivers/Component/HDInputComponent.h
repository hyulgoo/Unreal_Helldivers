// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Define/HDDefine.h"
#include "GameData/HDInputActionData.h"
#include "HDInputComponent.generated.h"

enum class EHDCharacterInputAction : uint8;

UCLASS()
class HELLDIVERS_API UHDInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
    template<class UserClass, typename TriggeredFuncType, typename ReleasedFuncType, typename ToggledFuncType>
    void SetTaggedInputActionDataAsset(UHDInputData* DataAsset, UserClass* Object, TriggeredFuncType TriggeredFunc, ReleasedFuncType ReleasedFunc, ToggledFuncType ToggledFunc);
};

template<class UserClass, typename TriggeredFuncType, typename ReleasedFuncType, typename ToggledFuncType>
void UHDInputComponent::SetTaggedInputActionDataAsset(UHDInputData* DataAsset, UserClass* Object, TriggeredFuncType TriggeredFunc, ReleasedFuncType ReleasedFunc, ToggledFuncType ToggledFunc)
{
    NULL_CHECK(DataAsset);

    for (const FHDInputAction& InputAction : DataAsset->AbilityInputActions)
    {
        if (InputAction.TriggerType == EHDTriggerType::Hold)
        {
            if (TriggeredFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, TriggeredFunc, InputAction.InputTag);
            }

            if (ReleasedFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputAction.InputTag);
            }
        }
        else if (InputAction.TriggerType == EHDTriggerType::Toggle)
        {
            if (ToggledFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, ToggledFunc, InputAction.InputTag);
            }
        }
        else if (InputAction.TriggerType == EHDTriggerType::Trigger)
        {
            if (TriggeredFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, TriggeredFunc, InputAction.InputTag);
            }
        }
    }
}