// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDProjectile.h"
#include "HDProjectileGrenade.generated.h"

UCLASS()
class HELLDIVERS_API AHDProjectileGrenade : public AHDProjectile
{
	GENERATED_BODY()
	
public:
	explicit AHDProjectileGrenade();

	virtual void Destroyed() override final;

protected:
	virtual void BeginPlay() override final;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> BounceSound;
};
