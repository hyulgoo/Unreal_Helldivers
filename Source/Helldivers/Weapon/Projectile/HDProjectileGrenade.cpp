// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Projectile/HDProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Define/HDDefine.h"

AHDProjectileGrenade::AHDProjectileGrenade()
    : BounceSound(nullptr)
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement->SetIsReplicated(true);
    ProjectileMovement->bShouldBounce = true;
}

void AHDProjectileGrenade::Destroyed()
{
    Super::Destroyed();
}

void AHDProjectileGrenade::BeginPlay()
{
    AActor::BeginPlay();

    ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AHDProjectileGrenade::OnBounce);
}

void AHDProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
    NULL_CHECK(BounceSound);

    UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
}
