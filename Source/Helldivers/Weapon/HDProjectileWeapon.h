// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HDWeapon.h"
#include "HDProjectileWeapon.generated.h"

class AHDProjectileBase;

UCLASS()
class HELLDIVERS_API AHDProjectileWeapon : public AHDWeapon
{
    GENERATED_BODY()

public:
    explicit     AHDProjectileWeapon();

    virtual void Fire(const FVector& HitTarget) override final;
};
