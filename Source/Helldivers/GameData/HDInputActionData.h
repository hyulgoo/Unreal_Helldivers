#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "HDInputActionData.generated.h"

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
struct FHDInputAction
{
    GENERATED_BODY()

public:
    FHDInputAction();
    FHDInputAction(UInputAction* NewInputAction, const FGameplayTag& NewInputTag, const FGameplayTag& NewAbilityTriggerTag, const EHDTriggerType  NewTriggerType);

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>    InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag                InputTag;    
    
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag				AbilityTriggerTag;
    
	UPROPERTY(EditDefaultsOnly)
	EHDTriggerType				TriggerType;
};

UCLASS()
class HELLDIVERS_API UHDInputData : public UDataAsset
{
	GENERATED_BODY()

public:
    explicit UHDInputData();

    const FGameplayTag GetInputTagByTriggerTag(const FGameplayTag& TriggerTag);
    const FGameplayTag GetTriggerTagByInputTag(const FGameplayTag& InputTag);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHDInputAction> AbilityInputActions;
};