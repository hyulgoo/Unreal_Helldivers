// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDWeaponInterface.generated.h"

class AHDWeapon;
class UAnimMontage;
class AHDProjectileBase;
enum class EHDCombatState : uint8;
enum class EHDCombatMontage : uint8;

UINTERFACE(MinimalAPI)
class UHDWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDWeaponInterface
{
	GENERATED_BODY()

public:
    virtual void                            EquipWeapon(AHDWeapon* NewWeapon) = 0;
    virtual UAnimMontage*                   GetCombatMontage(const EHDCombatMontage MontageType) const = 0;
    
    virtual void                            PlayWeaponMontage(const EHDCombatMontage MontageType) = 0;
    virtual bool                            IsEquippedWeapon() const = 0;
    
    virtual bool                            IsShoulder() const = 0;       
    virtual bool                            IsWeaponAutoFire() const = 0;
    virtual void                            SpawnProjectile() = 0;
	virtual float	                        GetWeaponFireDelay() const = 0;
	virtual void		                    SetWeaponActive(const bool bActive) = 0;
};
