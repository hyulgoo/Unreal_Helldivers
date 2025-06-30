// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponTypes.h"
#include "HDWeapon.generated.h"

UENUM()
enum class ECrosshair : uint8
{
    Center,
    Left,
    Right,
    Top,
    Bottom,
    Count
};

UENUM(BlueprintType)
enum class EHDWeaponAnimationType: uint8
{
   Fire_Aim,
   Fire_Hip,
   Reload_Aim,
   Reload_Hip,
   Count
};

class USphereComponent;
class UHDWeaponAttributeSet;
class UTexture2D;
class USoundCue;
class UAnimationAsset;
class AHDCasing;
class AHDProjectileBase;

UCLASS()
class HELLDIVERS_API AHDWeapon : public AActor
{
    GENERATED_BODY()

public:
    explicit                            AHDWeapon();

    void                                SetAutoFire(const bool bAutoFire);
    void                                SetWeaponState(const EWeaponState NewState);

    USkeletalMeshComponent*             GetWeaponMesh() const;
    USphereComponent*                   GetAreaSphere() const;

    virtual void                        Fire(const FVector& HitTarget, const bool bIsShoulder);
    const void                          TraceEndWithScatter(const FVector& HitTarget);

    const EWeaponType                   GetWeaponType() const;
    const EHDFireType                   GetFireType() const;
    const bool                          IsAmmoEmpty() const;
    const bool                          IsAmmoFull() const;
    const bool                          IsCapacityEmpty() const;
    const bool                          IsCapacityFull() const;
    const bool                          IsUseScatter() const;
    const bool                          IsAutoFire() const;
    const float                         GetFireDelay() const;
    const float                         GetErgonomicFactor() const;
    const int32                         GetAmmoCount() const;
    const int32                         GetMaxAmmoCount() const;
    const int32                         GetCapacityCount() const;
    const int32                         GetMaxCapacityCount() const;
    const float                         GetReloadDelay(const bool bIsShoulder) const;
    UTexture2D*                         GetWeaponIconImage() const;   
    const float                         GetZoomedFOV() const;
    const float                         GetZoomInterpSpeed() const;

    void                                Reload(const bool bIsShoulder);
    void                                AddCapacity(const int32 NewCapacityCount);

protected:
    virtual void                        BeginPlay() override;

    UFUNCTION()
    void                                OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult);

private:
    void                                SpendRound();
    void                                ReloadFinished();

protected:
    EHDFireType                         FireType;

    UPROPERTY(EditAnywhere, Category = "Weapon|Crosshair")
    TMap<ECrosshair, TObjectPtr<UTexture2D>>  CrosshaiTextureList;

    UPROPERTY(EditAnywhere, Category = "Weapon|Zoom")
    float                               ZoomedFOV;     
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Zoom")
    float                               ZoomInterpSpeed;

    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    TSubclassOf<AHDProjectileBase>      ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    EWeaponType							WeaponType;

    UPROPERTY(VisibleAnywhere, Category = "Weapon|Default")
    TObjectPtr<USkeletalMeshComponent>	WeaponMesh;

    UPROPERTY(VisibleAnywhere, Category = "Weapon|Properties")
    TObjectPtr<USphereComponent>		AreaSphere;

    UPROPERTY(EditAnywhere, Category = "Weapon|Animation")
    TMap<EHDWeaponAnimationType,TObjectPtr<UAnimationAsset>> WeaponAnimationMap;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    TObjectPtr<USoundCue>               EquipSound;

    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    TSubclassOf<AHDCasing>				CasingClass;

    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    TObjectPtr<UTexture2D>				WeaponIconImage;
        
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    float                               FireDelay;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    float                               ErgonomicFactor;

    UPROPERTY(EditAnywhere, Category = "Weapon|Current")
    int32                               Ammo;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    int32                               MaxAmmo;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Current")
    int32                               Capacity;

    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    int32                               MaxCapacity;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Current")
    uint8                               bUseScatter : 1;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    uint8                               bIsAutoFire : 1;

    EWeaponState                        WeaponState;
    FTimerHandle                        ReloadTimer;
};
