// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/HDProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile/HDProjectile.h"
#include "Projectile/HDProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Define/HDDefine.h"

AHDProjectileWeapon::AHDProjectileWeapon()
{
    FireType = EFireType::Projectile;
}

void AHDProjectileWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);

    NULL_CHECK(ProjectileClass);

    const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
    NULL_CHECK(MuzzleFlashSocket);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMesh);
    const FVector ToTarget = HitTarget - SocketTransform.GetLocation();
    const FTransform SpawnTransform(ToTarget.Rotation(), SocketTransform.GetLocation());

    AHDProjectile* SpawnedProjectile = World->SpawnActorDeferred<AHDProjectile>(
        ProjectileClass,
        SpawnTransform,
        nullptr,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    SpawnedProjectile->Damage               = Damage;
    SpawnedProjectile->HeadShotDamageRate   = HeadShotDamageRate;
    SpawnedProjectile->InitialSpeed         = ProjectileSpeed;

    UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnTransform);
}