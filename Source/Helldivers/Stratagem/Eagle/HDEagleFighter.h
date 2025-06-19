// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "GameData/HDStratagemEffectData.h"
#include "HDEagleFighter.generated.h"

class USplineComponent;
class AHDProjectileBase;
class USphereComponent;

UCLASS()
class HELLDIVERS_API AHDEagleFighter : public AActor
{
    GENERATED_BODY()

    friend class AHDBattleShip;

public:
    explicit                            AHDEagleFighter();

protected:
    virtual void                        BeginPlay() override final;

private:
    UFUNCTION()
    void                                HandleTimelineProgress(const float Value);

    UFUNCTION()
    void                                HandleTimelineFinished();

    void                                InitEagleFighter();
    void                                SetSplnePoints();
    void                                SetActiveEagleFighter(const bool bIsActive);
    void                                DropBombWithDelayAndReturn(const int32 Index);
    void                                CreateProjectile(TSubclassOf<AHDProjectileBase> ProjectileClass, const int32 Index);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Eagle")
    TObjectPtr<UStaticMeshComponent>    EagleFighterMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = "Eagle")
	TObjectPtr<USphereComponent>		CollisionSphere;

    UPROPERTY()
    TObjectPtr<USplineComponent>		SplineComponent;

    UPROPERTY()
    TObjectPtr<UTimelineComponent>		TimelineComponent;

    UPROPERTY(EditAnywhere, Category = "Stratagem|Curve")
    TObjectPtr<UCurveFloat>             DescentCurve;

    FOnTimelineFloat                    InterpFunction;
    FOnTimelineEvent                    TimelineFinished;

    FVector                             FlightStartLocation;
    FVector                             FlightDirection;
    uint8                               bIsAirStrikeDone : 1;
    float                               SplineLength;

    // Projectile
    UPROPERTY()
    TSubclassOf<AHDProjectileBase>	    ProjectileBombClass;
    
    UPROPERTY()
    TSubclassOf<AHDProjectileBase>	    ProjectileBulletClass;

    FHDStratagemEffectData              StratagemEffectData;
    FVector                             ProjectileTargetLocation;
    float                               ProjectileDurationofFlight;
};