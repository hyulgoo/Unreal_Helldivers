#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Stratagem/Types/StratagemProjectileTypes.h"
#include "Stratagem/Types/EagleAirStrikeDirectionTypes.h"
#include "HDStratagemEffectData.generated.h"

USTRUCT(BlueprintType)
struct FHDStratagemEffectData : public FTableRowBase
{
    GENERATED_BODY()

public:
    FHDStratagemEffectData()
        : bIsEagle(false)
        , EagleAirStrikeDirection(EHDEagleAirStrikeDirection::Count)
        , OrbitalDuration(0.f)
        , bUseRandomRange(false)
        , RandomPositionRange(0.f)
        , SpecifyProjectileSpawnCount(0)
        , SpecifyProjectileSpawnDelay(0.f)
        , bMultipleSpawn(false)
        , MultiSpawnDropLocation{}
        , StratagemProjectileType(EHDStratagemProjectile::Count)
        , ProjectileDropLocation{}
        , ProjectileAttackRange(0.f)
        , ProjectileDamage(0.f)
    {
    };

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eagle")
    uint8                       bIsEagle : 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eagle")
    EHDEagleAirStrikeDirection  EagleAirStrikeDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbital")
    float                       OrbitalDuration;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomLocation")
    uint8                       bUseRandomRange: 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomLocation")
    float                       RandomPositionRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecifyProjectile")
    uint8                       SpecifyProjectileSpawnCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecifyProjectile")
    float                       SpecifyProjectileSpawnDelay;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiple")
    uint8                       bMultipleSpawn: 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiple")
    TArray<FVector2D>           MultiSpawnDropLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    EHDStratagemProjectile      StratagemProjectileType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    TArray<FVector2D>           ProjectileDropLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    float                       ProjectileAttackRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    float                       ProjectileDamage;
};