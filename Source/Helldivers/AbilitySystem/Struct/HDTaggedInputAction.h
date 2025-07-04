#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDTaggedInputAction.generated.h"

class UInputAction;

USTRUCT()
struct FTaggedInputAction
{
    GENERATED_BODY()

public:
    FTaggedInputAction() 
        : InputAction(nullptr)
        , InputTag(FGameplayTag())
    { }

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>    InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag                InputTag;    
};
