// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "HDEagleFighter.generated.h"

class USplineComponent;
class AHDProjectile;
struct FHDStratagemEffectData;
enum class EHDEagleAirStrikeDirection : uint8;

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
    void            SetSplnePoints();
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

    FVector                             FlightStartLocation         = FVector();
    bool                                bIsAirStrikeDone            = false;
    float                               SplineLength                = 0.f;    

    // Projectile
    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectile>	        ProjectileBombClass;
    
    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectile>	        ProjectileBulletClass;

    FHDStratagemEffectData*             StratagemEffectData         = nullptr;
    FVector                             ProjectileTargetLocation    = FVector();
    float                               ProjectileDurationofFlight  = 1.f;

    EHDEagleAirStrikeDirection          EagleAirStrikeDirection;
};