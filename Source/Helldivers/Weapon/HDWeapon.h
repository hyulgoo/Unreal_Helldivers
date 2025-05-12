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
    explicit AHDWeapon();

    FORCEINLINE void                    SetAutoFire(const bool AutoFire) { bIsAutoFire = AutoFire; }
    FORCEINLINE void                    SetWeaponState(const EWeaponState NewState) { WeaponState = NewState; }

    FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
    FORCEINLINE USphereComponent*       GetAreaSphere() const { return AreaSphere; }

    virtual void                        Fire(const FVector& HitTarget);
    const FVector                       TraceEndWithScatter(const FVector& HitTarget);

    const EWeaponType                   GetWeaponType() const { return WeaponType; }
    const EFireType                     GetFireType() const { return FireType; }
    const bool                          IsAmmoEmpty() const;
    const bool                          IsAmmoFull() const;
    FORCEINLINE const bool              IsUseScatter() const { return bUseScatter; }
    FORCEINLINE const bool              IsAutoFire() const { return bIsAutoFire; }
    FORCEINLINE const float             GetFireDelay() const { return FireDelay; }
    FORCEINLINE const float             GetErgonomicFactor() const { return ErgonomicFactor; }
    FORCEINLINE const int32             GetAmmoCount() const { return Ammo; }
    FORCEINLINE const int32             GetMaxAmmoCount() const { return MaxAmmo; }
    FORCEINLINE const int32             GetCapacityCount() const { return Capacity; }
    FORCEINLINE const int32             GetMaxCapacityCount() const { return MaxCapacity; }
    UTexture2D*                         GetWeaponIconImage() const { return WeaponIconImage; }

    FORCEINLINE const float             GetZoomedFOV() const { return ZoomedFOV; }
    FORCEINLINE const float             GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

protected:
    virtual void                        BeginPlay() override;

    UFUNCTION()
    void                                OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult);

private:
    void                                SpendRound();

protected:
    EFireType                           FireType = EFireType::Count;

    UPROPERTY(EditAnywhere, Category = "Info|Crosshair", Meta = (AllowPrivateAccess = "true"))
    TMap<ECrosshair, TObjectPtr<UTexture2D>>  CrosshaiTextureList;

    UPROPERTY(EditAnywhere)
    float                               ZoomedFOV         = 30.f;

    UPROPERTY(EditAnywhere)
    float                               ZoomInterpSpeed   = 20.f;

    UPROPERTY(EditAnywhere, Category = "Info|Default")
    TSubclassOf<AHDProjectileBase>      ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Info|Default", Meta = (AllowPrivateAccess = "true"))
    EWeaponType							WeaponType;

    UPROPERTY(VisibleAnywhere, Category = "Info|Default")
    TObjectPtr<USkeletalMeshComponent>	WeaponMesh;

    UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
    TObjectPtr<USphereComponent>		AreaSphere;

    UPROPERTY(EditAnywhere, Category = "Info|Animation")
    TObjectPtr<UAnimationAsset>			FireAnimation;
    
    UPROPERTY(EditAnywhere, Category = "Info|Default")
    TObjectPtr<USoundCue>               EquipSound;

    UPROPERTY(EditAnywhere, Category = "Info|Default")
    TSubclassOf<AHDCasing>				CasingClass;

    UPROPERTY(EditAnywhere, Category = "Info|Default")
    TObjectPtr<UTexture2D>				WeaponIconImage;
        
    UPROPERTY(EditAnywhere, Category = "Info|Default", Meta = (AllowPrivateAccess = "true"))
    float                               FireDelay;
    
    UPROPERTY(EditAnywhere, Category = "Info|Default", Meta = (AllowPrivateAccess = "true"))
    float                               ErgonomicFactor;

    UPROPERTY(EditAnywhere, Category = "Info|Current", Meta = (AllowPrivateAccess = "true"))
    int32                               Ammo;
    
    UPROPERTY(EditAnywhere, Category = "Info|Default", Meta = (AllowPrivateAccess = "true"))
    int32                               MaxAmmo;
    
    UPROPERTY(EditAnywhere, Category = "Info|Current", Meta = (AllowPrivateAccess = "true"))
    int32                               Capacity;

    UPROPERTY(EditAnywhere, Category = "Info|Default", Meta = (AllowPrivateAccess = "true"))
    int32                               MaxCapacity;

    UPROPERTY(EditAnywhere, Category = "Info|Current", Meta = (AllowPrivateAccess = "true"))
    uint8                               bIsCanUseAutoFire : 1;
    
    UPROPERTY(EditAnywhere, Category = "Info|Current", Meta = (AllowPrivateAccess = "true"))
    uint8                               bUseScatter : 1;
    
    UPROPERTY(EditAnywhere, Category = "Info|Default", Meta = (AllowPrivateAccess = "true"))
    uint8                               bIsAutoFire : 1;

    EWeaponState                        WeaponState;
};
