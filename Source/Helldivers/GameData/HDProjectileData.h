#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Stratagem/Types/StratagemProjectileTypes.h"
#include "HDStratagemEffectData.generated.h"

class UGameplayEffect;
enum class EStatusEffect;
enum class EImpactType;

USTRUCT(BlueprintType)
struct FHDProjectileData: public FTableRowBase
{
    GENERATED_BODY()

public:
    FHDProjectileData() = default;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag                            ProjectileTag;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>>    ImpactGameEffectList;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStatusEffect                           StateEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float                                   ImpactDamage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float                                   DotDamage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float                                   StateDuration;
        
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EImpactType                             ImpactType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float                                   ExplodeDamageRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float                                   ExplodeKnockBackRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag                            ImpactCueTag;
};