// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDProjectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class UAnimationAsset;
class AHDCasing;
class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystem;
class USoundCue;
class UGameplayEffect;

UCLASS()
class HELLDIVERS_API AHDProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	explicit AHDProjectile();

protected:
	virtual void	BeginPlay() override;
	virtual void	Tick(float DeltaSeconds) override;
	virtual void	Destroyed() override;
	
	void			SpawnTrailSystem();
	void			ExplodeDamage();
	
	UFUNCTION()
	virtual void	OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void			StartDestroyTimer();
    void			DestroyTimerFinished();

public:	
	float			Damage				= 0.f;	
	float			HeadShotDamageRate	= 0.f;	
	float			InitialSpeed		= 15000.f;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent>			ProjectileMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent>	ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent>					CollisionBox;
	
	UPROPERTY(EditAnywhere)
	float										DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float										DamageOuterRadius = 500.f;
		
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem>					Tracer;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent>		TracerComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem>					TrailSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent>				TrailSystemComponent;

	UPROPERTY(EditAnywhere)
    TObjectPtr<UParticleSystem>					ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue>						ImpactSound;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect>				FireDamageEffect;

	float										ImpactParticlesScale;
	
private:
	UPROPERTY(EditAnywhere)
	float										DestroyTime = 3.f;

	FTimerHandle								DestroyTimer;
};
