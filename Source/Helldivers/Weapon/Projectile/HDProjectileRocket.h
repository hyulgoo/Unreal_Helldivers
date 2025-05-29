// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDProjectileBase.h"
#include "HDProjectileRocket.generated.h"

class USoundCue;
class UHDRocketMovementComponent;

UCLASS()
class HELLDIVERS_API AHDProjectileRocket : public AHDProjectileBase
{
	GENERATED_BODY()
	
public:
	explicit								AHDProjectileRocket();

protected:
    virtual void							BeginPlay() override final;
    virtual void							OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override final;
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue>					ProjectileLoop;

	UPROPERTY()
	TObjectPtr<UAudioComponent>				ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundAttenuation>			LoopingSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHDRocketMovementComponent>	RocketMovementComponent;
};
