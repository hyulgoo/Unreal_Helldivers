// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDProjectile.h"
#include "HDProjectileBullet.generated.h"

UCLASS()
class HELLDIVERS_API AHDProjectileBullet : public AHDProjectile
{
	GENERATED_BODY()
	
public:
	explicit AHDProjectileBullet();

protected:
    virtual void BeginPlay() override final;
	virtual void Tick(float DeltaSeconds) override final;
    virtual void OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override final;
};
