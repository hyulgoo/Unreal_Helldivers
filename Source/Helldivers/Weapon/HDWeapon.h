// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Weapon/WeaponTypes.h"
#include "GameData/HDWeaponData.h"
#include "HDWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpreadChanged, const float);

class USphereComponent;
class UAnimMontage;
class AHDCasing;
class AHDProjectileBase;

UCLASS()
class HELLDIVERS_API AHDWeapon : public AActor
{
    GENERATED_BODY()

    friend class UHDCombatComponent;

public:
    AHDWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void                                EquipWeapon(AActor* OwnerActor);
    const void                          TraceEndWithScatter(const FVector& HitTarget);
    void                                SpawnProjectile(const FVector& HitTarget);

private:
    UFUNCTION()
    void                                OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult);

public:
    FOnSpreadChanged                    OnSpreadChanged;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    TObjectPtr<USkeletalMeshComponent>	WeaponMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Properties")
    TObjectPtr<USphereComponent>		AreaSphere;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    EHDFireType                         FireType = EHDFireType::Count;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    TSubclassOf<AHDProjectileBase>      ProjectileClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    TSubclassOf<AHDCasing>				CasingClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation")
    TObjectPtr<UAnimMontage>            WeaponAnimMontage;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    FGameplayTag                        EquipSoundTag;
            
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    float                               FireDelay;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    uint8                               bIsAutoFire : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    FHDWeaponAmmoData                   AmmoData;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    FHDWeaponErgoData                   ErgoData;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
    FHDWeaponUIData                     UIData;

    EWeaponState                        WeaponState = EWeaponState::Drop;
};
