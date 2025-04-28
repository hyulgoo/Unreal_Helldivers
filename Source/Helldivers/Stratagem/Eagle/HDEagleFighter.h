// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "GameData/HDStratagemEffectData.h"
#include "HDEagleFighter.generated.h"

class USplineComponent;
class AHDProjectile;

UCLASS()
class HELLDIVERS_API AHDEagleFighter : public AActor
{
    GENERATED_BODY()

    friend class AHDBattleShip;

public:
    explicit AHDEagleFighter();

protected:
    virtual void    BeginPlay() override final;

private:
    UFUNCTION()
    void            HandleTimelineProgress(const float Value);

    UFUNCTION()
    void            HandleTimelineFinished();

    void            InitEagleFlighter();
    void            SetSplnePoints(const bool bReturn);
    void            SetActiveEagleFlighter(const bool bIsActive);
    void            DropBombWithDelayAndReturn(const int32 Index);
    void            CreateProjectile(TSubclassOf<AHDProjectile> ProjectileClass, const int32 Index);

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USplineComponent>		SplineComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTimelineComponent>		TimelineComponent;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UCurveFloat>             DescentCurve;

    FOnTimelineFloat                    InterpFunction;
    FOnTimelineEvent                    TimelineFinished;

    FVector                             FlightStartLocation;
    FVector                             FlightDirection;
    bool                                bIsAirStrikeDone;
    float                               SplineLength;

    // Projectile
    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectile>	        ProjectileBombClass;
    
    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectile>	        ProjectileBulletClass;

    FHDStratagemEffectData              StratagemEffectData;
    FVector                             ProjectileTargetLocation;
    float                               ProjectileDurationofFlight;
};