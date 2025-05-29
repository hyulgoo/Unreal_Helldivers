// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDProjectileBase.h"
#include "HDProjectileGrenade.generated.h"

class USoundCue;

UCLASS()
class HELLDIVERS_API AHDProjectileGrenade : public AHDProjectileBase
{
	GENERATED_BODY()
	
public:
	explicit		AHDProjectileGrenade();

	virtual void	Destroyed() override final;

protected:
	virtual void	BeginPlay() override final;

	UFUNCTION()
	void			OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundCue> BounceSound;
};
