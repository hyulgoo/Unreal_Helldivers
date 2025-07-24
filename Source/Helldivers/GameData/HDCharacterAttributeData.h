#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HDCharacterAttributeData.generated.h"

USTRUCT(BlueprintType)
struct FHDCharacterAttributeData : public FTableRowBase
{
	GENERATED_BODY()

public:    
    UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float>  Data;
};
