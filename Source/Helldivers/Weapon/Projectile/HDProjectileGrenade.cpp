// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Projectile/HDProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AHDProjectileGrenade::AHDProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AHDProjectileGrenade::Destroyed()
{
	ExplodeDamage();
	Super::Destroyed();
}

void AHDProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	SpawnTrailSystem();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AHDProjectileGrenade::OnBounce);
}

void AHDProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
	}
}
