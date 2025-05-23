#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HDStratagemData.generated.h"

enum class EHDCommandInput : uint8;

USTRUCT(BlueprintType)
struct FHDStratagemData : public FTableRowBase
{
    GENERATED_BODY()

public:
    FHDStratagemData()
        : CommandSequence{}
        , StratagemDelay(0.f)
    {
    }

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EHDCommandInput> CommandSequence;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float                   StratagemDelay;
};