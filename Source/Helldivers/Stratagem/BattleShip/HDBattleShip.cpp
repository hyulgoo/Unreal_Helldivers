// Fill out your copyright notice in the Description page of Project Settings.

#include "Stratagem/BattleShip/HDBattleShip.h"
#include "Stratagem/Eagle/HDEagleFighter.h"
#include "Stratagem/Types/StratagemProjectileTypes.h"
#include "GameData/HDStratagemEffectData.h"
#include "Game/HDGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Projectile/HDProjectileBase.h"
#include "Define/HDDefine.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

AHDBattleShip::AHDBattleShip()
    : BattleShipMesh(nullptr)
	, CollisionSphere(nullptr)
    , AbilitySystemComponent(nullptr)
    , EagleFighterClass(nullptr)
    , EagleFighter(nullptr)
    , ProjectileBombClass(nullptr)
    , ProjectileBulletClass(nullptr)
    , StratagemEffectDataTable(nullptr)
    , StratagemTransform(FTransform())
    , ActiveStratagemTimerHandle(FTimerHandle())
    , CurrentStratagemIndex(0)
    , bCanUseStratagem(false)
{
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    SetRootComponent(CollisionSphere);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    BattleShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BattleShipMesh"));
    BattleShipMesh->SetupAttachment(RootComponent);
    BattleShipMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void AHDBattleShip::ActivateStratagem(const FName StratagemName, const FTransform& Transform, const float StratagemActiveDelay)
{
    FHDStratagemEffectData* StratagemEffectData = FindStratagemEffectData(StratagemName);
    NULL_CHECK(StratagemEffectData);

    StratagemTransform = Transform;
    ActiveStratagemTimerHandle.Invalidate();

    if (StratagemEffectData->bIsEagle)
    {
        FTimerDelegate Delegate = FTimerDelegate::CreateLambda(
            [this, StratagemEffectData, Transform]()
            {
                EagleStrike(*StratagemEffectData);
            }
        );

        if(Delegate.IsBound())
        {
            GetWorldTimerManager().SetTimer(
                ActiveStratagemTimerHandle,
                Delegate,
                StratagemActiveDelay,
                false
            );
        }
    }
    else
    {
        const int32 BombIndex = 0;
        FTimerDelegate Delegate = FTimerDelegate::CreateLambda(
            [this, StratagemEffectData, BombIndex]()
            {
                OrbitalStrikeWithDelay(*StratagemEffectData, BombIndex);
            }
        );

        if (Delegate.IsBound())
        {
            GetWorldTimerManager().SetTimer(
                ActiveStratagemTimerHandle,
                Delegate,
                StratagemActiveDelay,
                false
            );
        }
    }
}

UAbilitySystemComponent* AHDBattleShip::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHDBattleShip::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    AHDGameState* GameState = Cast<AHDGameState>(World->GetGameState());
    VALID_CHECK(GameState);

    GameState->SetBattleShip(this);
}

FHDStratagemEffectData* AHDBattleShip::FindStratagemEffectData(const FName StratagemName) const
{
    NULL_CHECK_WITH_RETURNTYPE(StratagemEffectDataTable, nullptr);

    FHDStratagemEffectData* StratagemData = StratagemEffectDataTable->FindRow<FHDStratagemEffectData>(StratagemName, TEXT("Lookup"));
    NULL_CHECK_WITH_RETURNTYPE(StratagemData, nullptr);

    return StratagemData;
}

void AHDBattleShip::OrbitalStrikeWithDelay(const FHDStratagemEffectData& StratagemEffectData, int32 BombIndex)
{
    NULL_CHECK(StratagemEffectDataTable);

    const int32 NumberOfProjectileToBeSpawn = StratagemEffectData.ProjectileDropLocation.IsEmpty() 
        ? StratagemEffectData.SpecifyProjectileSpawnCount : StratagemEffectData.ProjectileDropLocation.Num();
    CONDITION_CHECK(BombIndex > NumberOfProjectileToBeSpawn);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    TSubclassOf<AHDProjectileBase> ProjectileClass = StratagemEffectData.StratagemProjectileType == EHDStratagemProjectile::Bullet 
                                                    ? ProjectileBulletClass : ProjectileBombClass;
    NULL_CHECK(ProjectileClass);

    FVector FinalDropTargetPosition = FVector();
    if (StratagemEffectData.bUseRandomRange)
    {
        const float Angle       = FMath::FRandRange(0.f, 2.f * PI);
        const float Distance    = FMath::Sqrt(FMath::FRand()) * StratagemEffectData.RandomPositionRange;
        const float RandomX     = FMath::Cos(Angle) * Distance;
        const float RandomY     = FMath::Sin(Angle) * Distance;
        FinalDropTargetPosition = StratagemTransform.GetLocation() + FVector(RandomX, RandomY, 0.f);
    }
    else
    {
        CONDITION_CHECK(StratagemEffectData.ProjectileDropLocation.IsEmpty());

        const FVector2D& DropPosition2D = StratagemEffectData.ProjectileDropLocation[BombIndex];
        FinalDropTargetPosition = StratagemTransform.GetLocation() + FVector(DropPosition2D.X, DropPosition2D.Y, 0.f);
    }

    const FVector& SpawnLocation    = GetActorLocation();
    const FVector ToTarget          = FinalDropTargetPosition - SpawnLocation;
    const FTransform SpawnTransform(ToTarget.Rotation(), SpawnLocation);

    AHDProjectileBase* SpawnProjectile = World->SpawnActorDeferred<AHDProjectileBase>(
        ProjectileClass,
        SpawnTransform,
        this,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
    );

    UGameplayStatics::FinishSpawningActor(SpawnProjectile, SpawnTransform);

    if (BombIndex + 1 < NumberOfProjectileToBeSpawn)
    {
        FTimerHandle TimerHandle;
        const int32 NextBombIndex = BombIndex + 1;
        FTimerDelegate Delegate = FTimerDelegate::CreateLambda(
            [this, StratagemEffectData, NextBombIndex]()
            {
                OrbitalStrikeWithDelay(StratagemEffectData, NextBombIndex);
            });
        World->GetTimerManager().SetTimer(
            TimerHandle,
            Delegate,
            StratagemEffectData.OrbitalDuration / NumberOfProjectileToBeSpawn,
            false
        );
    }
}

void AHDBattleShip::EagleStrike(const FHDStratagemEffectData& StratagemEffectData)
{
    NULL_CHECK(EagleFighterClass);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    CONDITION_CHECK(StratagemTransform.IsValid() == false);

    const FVector SpawnLocation = GetActorLocation();
    if (IsValid(EagleFighter))
    {
        EagleFighter->FlightStartLocation = SpawnLocation;
        EagleFighter->StratagemEffectData = StratagemEffectData;
        EagleFighter->ProjectileTargetLocation = StratagemTransform.GetLocation();

        EagleFighter->SetActiveEagleFighter(true);
    }
    else
    {
        EagleFighter = World->SpawnActorDeferred<AHDEagleFighter>(
            EagleFighterClass,
            StratagemTransform,
            this,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
        );

        NULL_CHECK(EagleFighter);

        EagleFighter->FlightStartLocation = SpawnLocation;
        EagleFighter->StratagemEffectData = StratagemEffectData;
        EagleFighter->ProjectileTargetLocation = StratagemTransform.GetLocation();
        EagleFighter->ProjectileBombClass = ProjectileBombClass;
        EagleFighter->ProjectileBulletClass = ProjectileBulletClass;

        UGameplayStatics::FinishSpawningActor(EagleFighter, StratagemTransform);
    }
}
