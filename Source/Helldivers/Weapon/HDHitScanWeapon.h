// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HDWeapon.h"
#include "HDHitScanWeapon.generated.h"

class UParticleSystem;
class USoundCue;
class UGameplayEffect;

UCLASS()
class HELLDIVERS_API AHDHitScanWeapon : public AHDWeapon
{
    GENERATED_BODY()

public:
    explicit AHDHitScanWeapon();

    virtual void    Fire(const FVector& HitTarget) override final;

protected:
    void            WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

private:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UParticleSystem>     ImpactParticles;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundCue>           HitSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UParticleSystem>     BeamParticles;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UParticleSystem>     MuzzleFlash;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundCue>           FireSound;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect>    FireDamageEffect;
};
