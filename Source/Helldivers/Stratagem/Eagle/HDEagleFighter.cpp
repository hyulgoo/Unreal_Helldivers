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
    InitEagleFlighter();
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
    SetActiveEagleFlighter(false);
}

void AHDEagleFighter::InitEagleFlighter()
{
    bIsAirStrikeDone = false;
    SetSplnePoints();
    TimelineComponent->PlayFromStart();
}

void AHDEagleFighter::SetSplnePoints()
{
    NULL_CHECK(SplineComponent);

    FVector Direction = FVector::ZeroVector;
    switch (EagleAirStrikeDirection)
    {
    case EHDEagleAirStrikeDirection::Front:
        Direction = GetActorRotation().Vector();
        break;
    case EHDEagleAirStrikeDirection::Left:
        Direction = -FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);
        break;
    case EHDEagleAirStrikeDirection::Right:
        Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);
        break;
    case EHDEagleAirStrikeDirection::Back:
        Direction = -GetActorRotation().Vector();
        break;
    }

    if(EagleAirStrikeDirection == EHDEagleAirStrikeDirection::Count)
    {
        UE_LOG(LogTemp, Warning, TEXT("EagleAirStrikeDirection is Invalid!"));
        return;
    }

    const FVector Start = FlightStartLocation;
    const FVector Mid   = ProjectileTargetLocation + (FVector::UpVector * 10000.f);

    const FVector HeightOffset  = FVector(0.f, 0.f, (Start.Z - Mid.Z) / 4.f);
    const FVector ArcOffset     = Direction * 35000.f;

    const FVector MidPoint1 = (HeightOffset * 3.f) + ArcOffset;
    const FVector MidPoint2 = (HeightOffset * 2.f) + (ArcOffset * 2.f);
    const FVector MidPoint3 = HeightOffset + ArcOffset;
    const FVector MidPoint4 = HeightOffset + (ArcOffset * -1.f);
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

void AHDEagleFighter::SetActiveEagleFlighter(const bool bIsActive)
{
    SetActorHiddenInGame(bIsActive == false);
    SetActorTickEnabled(bIsActive);
    SetActorEnableCollision(bIsActive);

    if(bIsActive)
    {
        InitEagleFlighter();
    }
}

void AHDEagleFighter::DropBombWithDelayAndReturn(const int32 Index)
{
    bIsAirStrikeDone = true;

    NULL_CHECK(StratagemEffectData);

    int32 NumberOfProjectileToBeSpawn = StratagemEffectData->SpecifyProjectileSpawnCount;
    if (StratagemEffectData->ProjectileDropLocation.IsEmpty() == false)
    {
        NumberOfProjectileToBeSpawn = StratagemEffectData->ProjectileDropLocation.Num();
    }

    if(NumberOfProjectileToBeSpawn < 1)
    {
        UE_LOG(LogTemp, Error, TEXT("NumberOfProjectileToBeSpawn is Zero!"));
        return;
    }

    if (Index > NumberOfProjectileToBeSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("Index Invalid!"));
        return;
    }

    TSubclassOf<AHDProjectile> ProjectileClass = StratagemEffectData->StratagemProjectileType == EHDStratagemProjectile::Bullet 
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
    else if(Index + 1 == NumberOfProjectileToBeSpawn)
    {
        SetSplnePoints();
        TimelineComponent->PlayFromStart();
    }
}

void AHDEagleFighter::CreateProjectile(TSubclassOf<AHDProjectile> ProjectileClass, const int32 ProjectileIndex)
{
    UWorld* World = GetWorld();
    VALID_CHECK(World);

    const FVector& EagleLocation = GetActorLocation();
    const FVector2D& DropLocation2D = StratagemEffectData->ProjectileDropLocation[ProjectileIndex];

    FVector FinalDropTargetLocation = ProjectileTargetLocation + FVector(DropLocation2D.X, DropLocation2D.Y, 0.f);
    FVector ToTarget = FinalDropTargetLocation - EagleLocation;
    float Impulse = ToTarget.Length() * (1.f + (1.f - ProjectileIndex * StratagemEffectData->SpecifyProjectileSpawnDelay));
    FTransform SpawnTransform(ToTarget.Rotation(), EagleLocation);

    AHDProjectile* SpawnProjectile = World->SpawnActorDeferred<AHDProjectile>(
        ProjectileClass,
        SpawnTransform,
        nullptr,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
    );

    SpawnProjectile->Damage = StratagemEffectData->ProjectileDamage;
    SpawnProjectile->InitialSpeed = Impulse;

    UGameplayStatics::FinishSpawningActor(SpawnProjectile, SpawnTransform);

    if (StratagemEffectData->bMultipleSpawn)
    {
        const int32 ProjectileMultiSpawnCount = StratagemEffectData->MultiSpawnDropLocation.Num();
        FVector2D MultipleSpawnLocationDiff = FVector2D();
        for (int32 Index = 0; Index < ProjectileMultiSpawnCount; ++Index)
        {
            MultipleSpawnLocationDiff   = DropLocation2D + StratagemEffectData->MultiSpawnDropLocation[Index];
            FinalDropTargetLocation     = ProjectileTargetLocation + FVector(MultipleSpawnLocationDiff.X, MultipleSpawnLocationDiff.Y, 0.f);
            ToTarget                    = FinalDropTargetLocation - EagleLocation;
            Impulse                     = ToTarget.Length() * (1.f + (1.f - ProjectileIndex * StratagemEffectData->SpecifyProjectileSpawnDelay));
            SpawnTransform              = FTransform(ToTarget.Rotation(), EagleLocation);

            SpawnProjectile = World->SpawnActorDeferred<AHDProjectile>(
                ProjectileClass,
                SpawnTransform,
                nullptr,
                nullptr,
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
            );
            NULL_CHECK(SpawnProjectile);

            SpawnProjectile->Damage = StratagemEffectData->ProjectileDamage;
            SpawnProjectile->InitialSpeed = Impulse;

            UGameplayStatics::FinishSpawningActor(SpawnProjectile, SpawnTransform);
        }
    }
}
