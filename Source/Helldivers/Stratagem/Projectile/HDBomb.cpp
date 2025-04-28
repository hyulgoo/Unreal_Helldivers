// Fill out your copyright notice in the Description page of Project Settings.

#include "Stratagem/Projectile/HDBomb.h"
#include "GameData/HDStratagemEffectData.h"
#include "GameFramework/ProjectileMovementComponent.h"

AHDBomb::AHDBomb()
{
    UStaticMeshComponent* BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
    BombMesh->SetupAttachment(RootComponent);

    ImpactParticlesScale = 5.f;
}
