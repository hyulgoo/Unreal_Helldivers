// Fill out your copyright notice in the Description page of Project Settings.

#include "Stratagem/Eagle/HDEagleFighter.h"
#include "Stratagem/Types/EagleAirStrikeDirectionTypes.h"
#include "Stratagem/Types/StratagemProjectileTypes.h"
#include "Components/SplineComponent.h"
#include "Weapon/Projectile/HDProjectile.h"
#include "GameData/HDStratagemEffectData.h"
#include "Kismet/GameplayStatics.h"
#include "Define/HDDefine.h"

#define EagleFighterAttackTimingRatio 0.45f

AHDEagleFighter::AHDEagleFighter()
    : InterpFunction()
    , TimelineFinished()
    , FlightStartLocation()
    , FlightDirection()
    , bIsAirStrikeDone(false)
    , SplineLength(0.f)
    , StratagemEffectData()
    , ProjectileTargetLocation()
    , ProjectileDurationofFlight(0.f)
{
    PrimaryActorTick.bCanEverTick = true;
    
    UStaticMeshComponent* EagleFighterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EagleFighterMesh"));
    SetRootComponent(EagleFighterMesh);
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
    TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

    InterpFunction.BindUFunction(this, TEXT("HandleTimelineProgress"));
    TimelineFinished.BindUFunction(this, TEXT("HandleTimelineFinished"));

    static ConstructorHelpers::FObjectFinder<UCurveFloat> DescentCurveRef(TEXT("/Script/Engine.CurveFloat'/Game/Helldivers/Blueprint/Stratagem/Curve/CF_EagleOrbit.CF_EagleOrbit'"));
    if(DescentCurveRef.Succeeded())
    {
        DescentCurve = DescentCurveRef.Object;
        TimelineComponent->AddInterpFloat(DescentCurve, InterpFunction);
        TimelineComponent->SetTimelineFinishedFunc(TimelineFinished);
    }
}

void AHDEagleFighter::BeginPlay()
{
    Super::BeginPlay();

    InitEagleFighter();
}

void AHDEagleFighter::HandleTimelineProgress(const float TimelineRatio)
{
    NULL_CHECK(SplineComponent);

    const float Distance = FMath::Clamp(TimelineRatio * SplineLength, 0.f, SplineLength);
    const FVector NewLocation  = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
    const FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

    SetActorLocationAndRotation(NewLocation, NewRotation);

    if(bIsAirStrikeDone == false && TimelineRatio > EagleFighterAttackTimingRatio)
    {
        DropBombWithDelayAndReturn(0);
    }
}

void AHDEagleFighter::HandleTimelineFinished()
{
    // 복귀 완료 후 비활성화
    SetActiveEagleFighter(false);
}

void AHDEagleFighter::InitEagleFighter()
{
    bIsAirStrikeDone = false;
    SetSplnePoints();
    TimelineComponent->PlayFromStart();
}

void AHDEagleFighter::SetSplnePoints()
{
    NULL_CHECK(SplineComponent);

    const FVector CurrentRotation = GetActorRotation().Vector();
    switch (StratagemEffectData.EagleAirStrikeDirection)
    {            
    case EHDEagleAirStrikeDirection::Front:
        FlightDirection = CurrentRotation;
        break;
    case EHDEagleAirStrikeDirection::Left:
        FlightDirection = FVector(CurrentRotation.Y, -CurrentRotation.X, 0.f);
        break;
    case EHDEagleAirStrikeDirection::Right:
        FlightDirection = FVector(-CurrentRotation.Y, CurrentRotation.X, 0.f);
        break;
    case EHDEagleAirStrikeDirection::Back:
        FlightDirection = -CurrentRotation;
        break;
    }

    CONDITION_CHECK(StratagemEffectData.EagleAirStrikeDirection == EHDEagleAirStrikeDirection::Count);

    UE_LOG(LogTemp, Warning, TEXT("FlightDirection : %s"), *FlightDirection.ToString());
    const FVector Start = FlightStartLocation;
    const FVector Mid = ProjectileTargetLocation + (FVector::UpVector * 10000.f);

    const FVector HeightOffset  = FVector(0.f, 0.f, (Start.Z - Mid.Z) / 4.f);
    const FVector ArcOffset     = FlightDirection * 35000.f;

    const FVector MidPoint1 = (HeightOffset * 3.f) + (ArcOffset * 1.f);
    const FVector MidPoint2 = (HeightOffset * 2.f) + (ArcOffset * 2.f);
    const FVector MidPoint3 = (HeightOffset * 1.f) + (ArcOffset * 1.f);
    const FVector MidPoint4 = (HeightOffset * 1.f) + (ArcOffset * -1.f);
    const FVector MidPoint5 = (HeightOffset * 2.f) + (ArcOffset * -2.f);
    const FVector MidPoint6 = (HeightOffset * 3.f) + (ArcOffset * -1.f);

    SplineComponent->ClearSplinePoints();
    SplineComponent->AddSplinePoint(Start,      ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(MidPoint1,  ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(MidPoint2,  ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(MidPoint3,  ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(Mid,        ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(MidPoint4,  ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(MidPoint5,  ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(MidPoint6,  ESplineCoordinateSpace::World);
    SplineComponent->AddSplinePoint(Start,      ESplineCoordinateSpace::World);
    SplineComponent->SetClosedLoop(false);

    const int32 SplinePointsNumber = SplineComponent->GetNumberOfSplinePoints();
    for (int32 index = 0; index < SplinePointsNumber; ++index)
    {
        SplineComponent->SetSplinePointType(index, ESplinePointType::CurveCustomTangent);
    }

    SplineComponent->UpdateSpline();
    SplineLength = SplineComponent->GetSplineLength();
}

void AHDEagleFighter::SetActiveEagleFighter(const bool bIsActive)
{
    SetActorHiddenInGame(bIsActive == false);
    SetActorTickEnabled(bIsActive);
    SetActorEnableCollision(bIsActive);

    if(bIsActive)
    {
        InitEagleFighter();
    }
}

void AHDEagleFighter::DropBombWithDelayAndReturn(const int32 Index)
{
    bIsAirStrikeDone = true;

    const int32 NumberOfProjectileToBeSpawn = StratagemEffectData.ProjectileDropLocation.IsEmpty() 
                                        ? StratagemEffectData.SpecifyProjectileSpawnCount : StratagemEffectData.ProjectileDropLocation.Num();

    CONDITION_CHECK(NumberOfProjectileToBeSpawn < 1);
    CONDITION_CHECK(Index > NumberOfProjectileToBeSpawn);

    TSubclassOf<AHDProjectile> ProjectileClass = StratagemEffectData.StratagemProjectileType == EHDStratagemProjectile::Bullet 
                                                    ? ProjectileBulletClass : ProjectileBombClass;
    CreateProjectile(ProjectileClass, Index);

    if (Index + 1 < NumberOfProjectileToBeSpawn)
    {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(
            TimerHandle,
            FTimerDelegate::CreateUObject(this, &AHDEagleFighter::DropBombWithDelayAndReturn, Index + 1),
            0.05f,
            false
        );
    }
}

void AHDEagleFighter::CreateProjectile(TSubclassOf<AHDProjectile> ProjectileClass, const int32 ProjectileIndex)
{
    UWorld* World = GetWorld();
    VALID_CHECK(World);

    const FVector& EagleLocation = GetActorLocation();
    const FVector2D& DropLocation2D = StratagemEffectData.ProjectileDropLocation[ProjectileIndex];

    FVector FinalDropTargetLocation = ProjectileTargetLocation + FVector(DropLocation2D.X, DropLocation2D.Y, 0.f);
    FVector ToTarget = FinalDropTargetLocation - EagleLocation;
    float Impulse = ToTarget.Length() * (1.f + (1.f - ProjectileIndex * StratagemEffectData.SpecifyProjectileSpawnDelay));
    FTransform SpawnTransform(ToTarget.Rotation(), EagleLocation);

    AHDProjectile* SpawnProjectile = World->SpawnActorDeferred<AHDProjectile>(
        ProjectileClass,
        SpawnTransform,
        nullptr,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
    );

    SpawnProjectile->Damage = StratagemEffectData.ProjectileDamage;
    SpawnProjectile->InitialSpeed = Impulse;

    UGameplayStatics::FinishSpawningActor(SpawnProjectile, SpawnTransform);

    if (StratagemEffectData.bMultipleSpawn)
    {
        FVector2D MultipleSpawnLocationDiff = FVector2D();
        for(const FVector2D& DropPosition : StratagemEffectData.MultiSpawnDropLocation)
        {
            MultipleSpawnLocationDiff = DropLocation2D + DropPosition;
            FinalDropTargetLocation = ProjectileTargetLocation + FVector(MultipleSpawnLocationDiff.X, MultipleSpawnLocationDiff.Y, 0.f);
            ToTarget = FinalDropTargetLocation - EagleLocation;
            Impulse = ToTarget.Length() * (1.f + (1.f - ProjectileIndex * StratagemEffectData.SpecifyProjectileSpawnDelay));
            SpawnTransform = FTransform(ToTarget.Rotation(), EagleLocation);

            SpawnProjectile = World->SpawnActorDeferred<AHDProjectile>(
                ProjectileClass,
                SpawnTransform,
                nullptr,
                nullptr,
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
            );
            NULL_CHECK(SpawnProjectile);

            SpawnProjectile->Damage = StratagemEffectData.ProjectileDamage;
            SpawnProjectile->InitialSpeed = Impulse;

            UGameplayStatics::FinishSpawningActor(SpawnProjectile, SpawnTransform);
        }
    }
}
