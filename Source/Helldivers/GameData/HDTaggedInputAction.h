#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "HDTaggedInputAction.generated.h"

class UInputAction;

UENUM(BlueprintType)
enum class EHDTriggerType : uint8 
{
	None,
    Trigger,
	Hold,
    Toggle,
};

USTRUCT(BlueprintType)
struct FHDTaggedInputAction
{
    GENERATED_BODY()

public:
    FHDTaggedInputAction()
        : InputAction(nullptr)
        , InputTag(FGameplayTag())
        , TriggerType(EHDTriggerType::None)
    { }

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>    InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag                InputTag;    
    
	UPROPERTY(EditDefaultsOnly)
	EHDTriggerType				TriggerType;
};

UCLASS()
class HELLDIVERS_API UHDInputDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public :
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHDTaggedInputAction> AbilityInputActions;
};