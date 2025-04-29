#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDTaggedInputAction.generated.h"

class UInputAction;
enum class ETriggerEvent : uint8;

USTRUCT()
struct FTaggedInputAction
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>    InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag                InputTag;    
};
