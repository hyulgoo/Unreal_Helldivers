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
     FHDWeaponErgoData()
         : ZoomedFOV(0.f)
         , ZoomInterpSpeed(0.f)
         , ErgonomicFactor(0.f)
     {
     }
 
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
     FHDWeaponAmmoData()
         : Ammo(0)
         , MaxAmmo(0)
         , Capacity(0)
         , MaxCapacity(0)
     {
     }
 
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
    FHDWeaponCrosshairPair()
        : Type(EHDCrosshair::Count)
        , Texture(nullptr)
    {
    }

public:
    UPROPERTY(EditDefaultsOnly)
    EHDCrosshair Type;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UTexture2D> Texture;
};

 USTRUCT(BlueprintType)
 struct FHDWeaponUIData : public FTableRowBase
 {
     GENERATED_BODY()
 
 public:
     FHDWeaponUIData()
         : WeaponIconImage(nullptr)
     {
     }
 
 public:
     UPROPERTY(EditDefaultsOnly)
     TObjectPtr<UTexture2D>		        WeaponIconImage;
 
     UPROPERTY(EditDefaultsOnly)
     TArray<FHDWeaponCrosshairPair>    CrosshairTextureList;
 };
