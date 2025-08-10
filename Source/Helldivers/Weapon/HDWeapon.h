// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Weapon/WeaponTypes.h"
#include "GameData/HDWeaponData.h"
#include "HDWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpreadChanged, const float);

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
class UAnimationAsset;
class UAbilitySystemComponent;
class AHDCasing;
class AHDProjectileBase;
class UHDWeaponAttributeSet;
struct FGameplayEventData;

UCLASS()
class HELLDIVERS_API AHDWeapon : public AActor
{
    GENERATED_BODY()

public:
    AHDWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void                                SetAutoFire(const bool bAutoFire);

    USkeletalMeshComponent*             GetWeaponMesh() const;
    USphereComponent*                   GetAreaSphere() const; 

    virtual void                        EquipWeapon(AActor* OwnerActor, UAbilitySystemComponent* ASC);

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
    void                                SpendRound();

    UFUNCTION()
    void                                OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult);

private:
    void                                SetWeaponState(const EWeaponState NewState);
    virtual void                        SpawnProjectile(const FGameplayEventData* Payload) {};
    void                                ReloadFinished();

public:
    FOnSpreadChanged                    OnSpreadChanged;

protected:
    EHDFireType                         FireType = EHDFireType::Count;

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
    FGameplayTag                        EquipSoundTag;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    TSubclassOf<AHDCasing>				CasingClass;
        
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    float                               FireDelay;
        
    UPROPERTY(EditAnywhere, Category = "Weapon|Current")
    uint8                               bUseScatter : 1;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    uint8                               bIsAutoFire : 1;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    FHDWeaponAmmoData                   AmmoData;
    
    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    FHDWeaponErgoData                   ErgoData;

    UPROPERTY(EditAnywhere, Category = "Weapon|Default")
    FHDWeaponUIData                     UIData;

    EWeaponState                        WeaponState = EWeaponState::Drop;
    FTimerHandle                        ReloadTimer;
    FVector                             CachedHitTarget;
};
