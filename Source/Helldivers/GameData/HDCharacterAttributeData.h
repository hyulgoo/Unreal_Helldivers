#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Define/HDGameplayTag.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "HDCharacterAttributeData.generated.h"

USTRUCT(BlueprintType)
struct FHDCharacterAttributeData : public FTableRowBase
{
	GENERATED_BODY()

public:
    FHDCharacterAttributeData()
    {
        const FGameplayTagContainer ChildTags = FGameplayAbilityHelper::GetAllChildTag(Tag_Data_Attribute);
        for (const FGameplayTag& ChildTag : ChildTags)
        {
            Data.Add(ChildTag, 0.f);
        }
    }

public:
    UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float>  Data;
};
