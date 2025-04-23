// Fill out your copyright notice in the Description page of Project Settings.

#include "Stratagem/HDStratagem.h"
#include "NiagaraFunctionLibrary.h"  
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "BattleShip/HDBattleShip.h"
#include "Kismet/GameplayStatics.h"
#include "Game/HDGameState.h"

AHDStratagem::AHDStratagem()
{
    PointLaserVisibleTime = 5.f;
    ThrowImpulse = 3000.f;

    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    SetRootComponent(CollisionSphere);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionSphere->OnComponentHit.AddDynamic(this, &AHDStratagem::OnHit);

    StratagemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StratagemMesh"));
    StratagemMesh->SetupAttachment(RootComponent);
    StratagemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHDStratagem::AddImpulseToStratagem(const FVector& NewThrowDirection)
{
    if (NewThrowDirection.IsNormalized() == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ThrowDirection is not Normalized!"));
        return;
    }

    DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
    SetOwner(nullptr);

    CollisionSphere->SetSimulatePhysics(true);
    CollisionSphere->SetNotifyRigidBodyCollision(true);
    ThrowDirection = NewThrowDirection;
    CollisionSphere->AddImpulse(ThrowDirection * ThrowImpulse);
}

void AHDStratagem::BeginPlay()
{
    Super::BeginPlay();
}

void AHDStratagem::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor)
    {
        const ECollisionChannel CollisionChannel = OtherComp->GetCollisionObjectType();
        if (CollisionChannel == ECollisionChannel::ECC_WorldStatic)
        {
            if (HitComp && HitComp->IsSimulatingPhysics())
            {
                AHDGameState* GameState = Cast<AHDGameState>(UGameplayStatics::GetGameState(GetWorld()));
                if(GameState == nullptr)
                {
                    UE_LOG(LogTemp, Error, TEXT("GameState is nullptr!"));
                    return;
                }

                AHDBattleShip* BattleShip = GameState->GetBattleShip();
                if (BattleShip == nullptr)
                {
                    UE_LOG(LogTemp, Error, TEXT("BattleShip is nullptr!"));
                    return;
                }

                const FVector& StratagemLocation = GetActorLocation();
                BattleShip->ActivateStratagem(StratagemName, StratagemLocation);

                SetActorLocation(Hit.ImpactPoint);
                HitComp->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
                HitComp->SetSimulatePhysics(false);
                SpawnPointLaser();
                SetLifeSpan(PointLaserVisibleTime);
            }
        }
    }
}

void AHDStratagem::SpawnPointLaser()
{
    UWorld* World = GetWorld();
    if(IsValid(World) == false)
    {
        ensureMsgf(false, TEXT("World is Invalid!"));
        return;
    }
    
    if(PointLaserNiagara == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("PointLaserNiagara is nullptr!"));
        return;
    }

    const FVector Start = FVector::ZeroVector;
    const FVector End = FVector::UpVector * 80000.f;
    NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, PointLaserNiagara, GetActorLocation());

    if (IsValid(NiagaraComponent) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("NiagaraComponent is nullptr!"));
        return;
    }

    NiagaraComponent->SetVectorParameter(TEXT("Beam Start"), Start);
    NiagaraComponent->SetVectorParameter(TEXT("Beam End"), End);
    NiagaraComponent->SetFloatParameter(TEXT("VisibleTime"), PointLaserVisibleTime);
}
