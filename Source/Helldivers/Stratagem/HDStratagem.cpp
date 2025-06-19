// Fill out your copyright notice in the Description page of Project Settings.

#include "Stratagem/HDStratagem.h"
#include "NiagaraFunctionLibrary.h"  
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "BattleShip/HDBattleShip.h"
#include "Kismet/GameplayStatics.h"
#include "Game/HDGameState.h"
#include "Define/HDDefine.h"
#include "Collision/HDCollision.h"

AHDStratagem::AHDStratagem()
    : StratagemMesh(nullptr)
    , CollisionSphere(nullptr)
    , PointLaserNiagara(nullptr)
    , NiagaraComponent(nullptr)
    , StratagemActiveDelay(0.f)
    , ThrowDirection(FVector())
    , ThrowImpulse(3000.f)
    , StratagemName(FName())
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
    CollisionSphere->SetCollisionProfileName(HDCOLLISION_PROFILE_STRATAGEM);
    CollisionSphere->IgnoreActorWhenMoving(GetInstigator(), true);
    CollisionSphere->OnComponentHit.AddDynamic(this, &AHDStratagem::OnHit);

    StratagemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StratagemMesh"));
    StratagemMesh->SetupAttachment(RootComponent);
    StratagemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHDStratagem::SetStratagemInfo(const FName SelectedStratagemName, const float SelectedStratagemActiveDelay)
{
    StratagemName = SelectedStratagemName;
    StratagemActiveDelay = SelectedStratagemActiveDelay;
}

void AHDStratagem::AddImpulseToStratagem(const FVector& NewThrowDirection)
{
    CONDITION_CHECK(NewThrowDirection.IsNormalized() == false);

    DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
    SetOwner(nullptr);

    ThrowDirection = NewThrowDirection;
    CollisionSphere->SetSimulatePhysics(true);
    CollisionSphere->SetNotifyRigidBodyCollision(true);
    CollisionSphere->AddImpulse(ThrowDirection * ThrowImpulse);
}

void AHDStratagem::BeginPlay()
{
    Super::BeginPlay();
}

void AHDStratagem::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherComp->GetCollisionObjectType() != ECC_WorldStatic)
    {
        return;
    }

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    AHDGameState* GameState = Cast<AHDGameState>(UGameplayStatics::GetGameState(World));
    NULL_CHECK(GameState);

    AHDBattleShip* BattleShip = GameState->GetBattleShip();
    NULL_CHECK(BattleShip);

    ThrowDirection.Z = 0.f;
    ThrowDirection.Normalize();

    const FTransform Transform(ThrowDirection.Rotation(), Hit.ImpactPoint);
    BattleShip->ActivateStratagem(StratagemName, Transform, StratagemActiveDelay);

    CollisionSphere->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    CollisionSphere->SetSimulatePhysics(false);
    SpawnPointLaser();

    SetLifeSpan(StratagemActiveDelay);
}

void AHDStratagem::SpawnPointLaser()
{
    UWorld* World = GetWorld();
    VALID_CHECK(World);

    NULL_CHECK(PointLaserNiagara);

    const FVector Start = FVector::ZeroVector;
    const FVector End = FVector::UpVector * 80000.f;
    NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, PointLaserNiagara, GetActorLocation());
    NULL_CHECK(NiagaraComponent);

    NiagaraComponent->SetVectorParameter(TEXT("Beam Start"), Start);
    NiagaraComponent->SetVectorParameter(TEXT("Beam End"),   End);
    NiagaraComponent->SetFloatParameter(TEXT("VisibleTime"), StratagemActiveDelay);
}
