// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Projectile/HDProjectileBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/HDCharacterBase.h"
#include "Controller/HDPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

AHDProjectileBullet::AHDProjectileBullet()
{
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

void AHDProjectileBullet::BeginPlay()
{
    Super::BeginPlay();

    StartDestroyTimer();
}

void AHDProjectileBullet::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AHDProjectileBullet::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnBoxHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}