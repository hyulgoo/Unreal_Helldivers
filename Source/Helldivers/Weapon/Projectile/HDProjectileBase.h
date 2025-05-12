// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "HDProjectileBase.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class UAnimationAsset;
class AHDCasing;
class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystem;
class USoundCue;
class UGameplayEffect;
enum class EImpactType : uint8;
enum class EStatusEffect : uint8;

UCLASS()
class HELLDIVERS_API AHDProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	explicit AHDProjectileBase();

protected:
	virtual void	BeginPlay() override;
	virtual void	Tick(float DeltaSeconds) override;
	virtual void	Destroyed() override;
	
	UFUNCTION()
	virtual void	OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    void			StartDestroyTimer();
    void			DestroyTimerFinished();
    void            SpawnTrailSystem();

public:	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Default")
    FGameplayTag                                ProjectileTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Default")
    float                                       InitSpeed;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Default")
    EImpactType                                 ImpactType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Default")
    TSubclassOf<UGameplayEffect>                ImpactGameEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Default")
    float                                       ImpactDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Status")
    EStatusEffect                               StatusEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Status")
    TSubclassOf<UGameplayEffect>                StatusGameEffect;
        
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Status")
    float                                       DotDamage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Status")
    float                                       StatusDuration;
            
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Explode")
    TSubclassOf<UGameplayEffect>                ExplodeGameEffect;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Explode")
    float                                       ExplodeDamageRange;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Explode")
    float                                       ExplodeKnockBackRange;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info|Explode")
    FGameplayTag                                ImpactCueTag;

protected:
	UPROPERTY(EditAnywhere, Category = "Info|Default")
	TObjectPtr<UStaticMeshComponent>			ProjectileMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent>	ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent>					CollisionBox;
	
private:        
    UPROPERTY(EditAnywhere, Category = "Info|Default")
    TObjectPtr<UParticleSystem>                 Tracer;

	UPROPERTY()
    TObjectPtr<UParticleSystemComponent>        TracerComponent;

    UPROPERTY(EditAnywhere, Category = "Info|Default")
    TObjectPtr<UNiagaraSystem>                  TrailSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraComponent>               TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category = "Info|Default")
	float										DestroyTime = 3.f;

	FTimerHandle								DestroyTimer;
};
