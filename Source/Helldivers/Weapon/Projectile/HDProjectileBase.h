// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "HDProjectileBase.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class AHDCasing;
class UNiagaraSystem;
class UNiagaraComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
enum class EImpactType : uint8;
enum class EStatusEffect : uint8;

UCLASS()
class HELLDIVERS_API AHDProjectileBase : public AActor
{
	GENERATED_BODY()

public:	
	explicit                                    AHDProjectileBase();

protected:
	virtual void	                            BeginPlay() override;
	virtual void	                            Tick(float DeltaSeconds) override;
	virtual void	                            Destroyed() override;
	
	UFUNCTION()
	virtual void	                            OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    void                                        InitializeBeginPlay();
    void			                            StartDestroyTimer();
    void			                            DestroyTimerFinished();
    void                                        SpawnTrailSystem();
    void                                        ApplyExplode(UAbilitySystemComponent* SourceAbiltySystemComponent, const FVector& HitLocation);

    void                                        ApplyDamageGameEffect(UAbilitySystemComponent* SourceAbiltySystemComponent, UAbilitySystemComponent* TargetAbiltySystemComponent, const float InterpImpactDamage);
    void                                        ApplyKnockbackGameAbility(UAbilitySystemComponent* SourceAbiltySystemComponent, UAbilitySystemComponent* TargetAbiltySystemComponent, const float InterpKnockbackImpulse);
    void                                        ExcuteGameplayCue(UAbilitySystemComponent* OwnerAbilitySystemComponent, const FGameplayTag& Tag, const FHitResult& Hit);
    
protected:
	UPROPERTY(EditAnywhere, Category = "Info|Default")
	TObjectPtr<UStaticMeshComponent>			ProjectileMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent>	ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent>					CollisionBox;

    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    FGameplayTag                                ProjectileTag;

    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    float                                       InitSpeed;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    EImpactType                                 ImpactType;

    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    TSubclassOf<UGameplayEffect>                DamageGameEffect;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    float                                       ImpactDamage;

    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    FGameplayTag                                ImpactBlocklCueTag;

    UPROPERTY(EditDefaultsOnly, Category = "Info|Default")
    FGameplayTag                                ImpactHitCueTag;

    UPROPERTY(EditDefaultsOnly, Category = "Info|Status")
    EStatusEffect                               StatusEffect;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Status")
    TSubclassOf<UGameplayEffect>                StatusGameEffect;
        
    UPROPERTY(EditDefaultsOnly, Category = "Info|Status")
    float                                       DotDamage;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Status")
    float                                       StatusDuration;
            
    UPROPERTY(EditDefaultsOnly, Category = "Info|Explode")
    float                                       ExplodeDamageRange;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Explode")
    float                                       ExplodeKnockBackRange;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Knockback")
    FGameplayTag                                KnockbackTag;
    
    UPROPERTY(EditDefaultsOnly, Category = "Info|Knockback")
    float                                       KnockbackImpulse;
    	
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
	float										DestroyTime = 10.f;

	FTimerHandle								DestroyTimer;
};
