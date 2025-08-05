#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HDWeaponData.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EHDCrosshair : uint8
{
    Center,
    Left,
    Right,
    Top,
    Bottom,
    Count
};
 
 USTRUCT(BlueprintType)
 struct FHDWeaponErgoData : public FTableRowBase
 {
     GENERATED_BODY()
 
 public:
     UPROPERTY(EditDefaultsOnly)
     float ZoomedFOV;     
     
     UPROPERTY(EditDefaultsOnly)
     float ZoomInterpSpeed;
     
     UPROPERTY(EditDefaultsOnly)
     float ErgonomicFactor;
 };
 
 USTRUCT(BlueprintType)
 struct FHDWeaponAmmoData : public FTableRowBase
 {
     GENERATED_BODY()
 
 public:
     UPROPERTY(EditDefaultsOnly)
     int32 Ammo;
     
     UPROPERTY(EditDefaultsOnly)
     int32 MaxAmmo;
     
     UPROPERTY(EditDefaultsOnly)
     int32 Capacity;
 
     UPROPERTY(EditDefaultsOnly)
     int32 MaxCapacity;
 };
 
 USTRUCT(BlueprintType)
struct FHDWeaponCrosshairPair
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    EHDCrosshair            Type;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UTexture2D>  Texture;
};

 USTRUCT(BlueprintType)
 struct FHDWeaponUIData : public FTableRowBase
 {
     GENERATED_BODY()
 
 public:
     UPROPERTY(EditDefaultsOnly)
     TObjectPtr<UTexture2D>		       WeaponIconImage;
 
     UPROPERTY(EditDefaultsOnly)
     TArray<FHDWeaponCrosshairPair>    CrosshairTextureList;
 };
