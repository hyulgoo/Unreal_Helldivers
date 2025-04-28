// Fill out your copyright notice in the Description page of Project Settings.

#include "Stratagem/BattleShip/HDBattleShip.h"
#include "Stratagem/Eagle/HDEagleFighter.h"
#include "Stratagem/Types/StratagemProjectileTypes.h"
#include "GameData/HDStratagemEffectData.h"
#include "Game/HDGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Projectile/HDProjectile.h"
#include "Define/HDDefine.h"

AHDBattleShip::AHDBattleShip()
    : StratagemTransform()
    , CurrentStratagemIndex(0)
    , ActiveStratagemTimerHandle()
    , bCanUseStratagem(false)
{
    UStaticMeshComponent* BattleShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BattleShipMesh"));
    SetRootComponent(BattleShipMesh);

    static ConstructorHelpers::FObjectFinder<UDataTable> StratagemDataListRef(TEXT("/Script/Engine.DataTable'/Game/Helldivers/GameData/DT_StratagemEffectData.DT_StratagemEffectData'"));
    if (StratagemDataListRef.Succeeded())
    {
        StratagemEffectDataTable = StratagemDataListRef.Object;
    }

    static ConstructorHelpers::FClassFinder<AHDEagleFighter> EagleFighterClassRef(TEXT("/Game/Helldivers/Blueprint/Stratagem/BP_EagleFighter.BP_EagleFighter_C"));
    if (EagleFighterClassRef.Class)
    {
        EagleFighterClass = EagleFighterClassRef.Class;
    }

    static ConstructorHelpers::FClassFinder<AHDProjectile> ProjectileBombClassRef(TEXT("/Game/Helldivers/Blueprint/Stratagem/BP_Bomb.BP_Bomb_C"));
    if (ProjectileBombClassRef.Class)
    {
        ProjectileBombClass = ProjectileBombClassRef.Class;
    }

    static ConstructorHelpers::FClassFinder<AHDProjectile> ProjectileBulletClassRef(TEXT("/Game/Helldivers/Blueprint/Stratagem/BP_EagleBullet.BP_EagleBullet_C"));
    if (ProjectileBulletClassRef.Class)
    {
        ProjectileBulletClass = ProjectileBulletClassRef.Class;
    }
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
    if (BombIndex > NumberOfProjectileToBeSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("BombIndex is Invalid!"));
        return;
    }

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    TSubclassOf<AHDProjectile> ProjectileClass = StratagemEffectData.StratagemProjectileType == EHDStratagemProjectile::Bullet 
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
        if(StratagemEffectData.ProjectileDropLocation.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("No RandomRangeUse but ProjectileDropLocationList is Empty!"));
            return;
        }

        const FVector2D& DropPosition2D = StratagemEffectData.ProjectileDropLocation[BombIndex];
        FinalDropTargetPosition = StratagemTransform.GetLocation() + FVector(DropPosition2D.X, DropPosition2D.Y, 0.f);
    }

    const FVector& SpawnLocation    = GetActorLocation();
    const FVector ToTarget          = FinalDropTargetPosition - SpawnLocation;
    const float Impulse             = ToTarget.Length();
    const FTransform SpawnTransform(ToTarget.Rotation(), SpawnLocation);

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

    if(StratagemTransform.IsValid() == false)
    {
        UE_LOG(LogTemp, Error, TEXT("StratagemTransform is Invalid!"));
        return;
    }

    const FVector SpawnLocation = GetActorLocation();
    if (IsValid(EagleFighter) == false)
    {
        EagleFighter = World->SpawnActorDeferred<AHDEagleFighter>(
            EagleFighterClass,
            StratagemTransform,
            nullptr,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
        );

        NULL_CHECK(EagleFighter);

        EagleFighter->FlightStartLocation       = SpawnLocation;
        EagleFighter->StratagemEffectData       = StratagemEffectData;
        EagleFighter->ProjectileTargetLocation  = StratagemTransform.GetLocation();
        EagleFighter->ProjectileBombClass       = ProjectileBombClass;
        EagleFighter->ProjectileBulletClass     = ProjectileBulletClass;

        UGameplayStatics::FinishSpawningActor(EagleFighter, StratagemTransform);
    }
    else
    {
        EagleFighter->FlightStartLocation       = SpawnLocation;
        EagleFighter->StratagemEffectData       = StratagemEffectData;
        EagleFighter->ProjectileTargetLocation  = StratagemTransform.GetLocation();

        EagleFighter->SetActiveEagleFighter(true);
    }
}
