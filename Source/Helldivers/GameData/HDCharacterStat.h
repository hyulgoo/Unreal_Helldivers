#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HDCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FHDCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:    
    UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float>  Stats;
};
