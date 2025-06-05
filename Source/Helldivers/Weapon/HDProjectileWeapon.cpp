// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/HDProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile/HDProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "Define/HDDefine.h"

AHDProjectileWeapon::AHDProjectileWeapon()
{
    FireType = EHDFireType::Projectile;
}

void AHDProjectileWeapon::Fire(const FVector& HitTarget, const bool bIsShoulder)
{
    Super::Fire(HitTarget, bIsShoulder);

    NULL_CHECK(ProjectileClass);

    const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
    NULL_CHECK(MuzzleFlashSocket);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMesh);
    const FVector ToTarget = HitTarget - SocketTransform.GetLocation();
    const FTransform SpawnTransform(ToTarget.Rotation(), SocketTransform.GetLocation());

    AActor* WeaponOwner = GetOwner();
    NULL_CHECK(WeaponOwner);

    AHDProjectileBase* SpawnedProjectile = World->SpawnActorDeferred<AHDProjectileBase>(
        ProjectileClass,
        SpawnTransform,
        WeaponOwner,
        Cast<APawn>(WeaponOwner));
    NULL_CHECK(SpawnedProjectile);
    UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnTransform);
}