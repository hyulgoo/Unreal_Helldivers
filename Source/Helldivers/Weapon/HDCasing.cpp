// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/HDCasing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AHDCasing::AHDCasing()
	: CasingMesh(nullptr)
	, ShellEjectionImpulse(10.f)
	, ShellSound(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
}

void AHDCasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &AHDCasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void AHDCasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}

	Destroy();
}
