// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Projectile/HDProjectileRocket.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Kismet/GameplayStatics.h"
#include "MovementComponent/HDRocketMovementComponent.h"

AHDProjectileRocket::AHDProjectileRocket()
	: ProjectileLoop(nullptr)
	, ProjectileLoopComponent(nullptr)
	, LoopingSoundAttenuation(nullptr)
	, RocketMovementComponent(nullptr)
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHDProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentHit.AddDynamic(this, &AHDProjectileRocket::OnBoxHit);

	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop, GetRootComponent(), FName(), GetActorLocation(),
			EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, LoopingSoundAttenuation, nullptr, false);
	}
}

void AHDProjectileRocket::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor == GetOwner())
	{
		return;
	}

	if(ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if(CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}
