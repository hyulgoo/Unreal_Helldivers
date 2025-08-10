// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/HDProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile/HDProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"

AHDProjectileWeapon::AHDProjectileWeapon(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    FireType = EHDFireType::Projectile;
}

void AHDProjectileWeapon::SpawnProjectile(const FGameplayEventData* Payload)
{
    NULL_CHECK(ProjectileClass);

    const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMesh->GetSocketByName(HDSOCKETNAME_MUZZLEFLASH);
    NULL_CHECK(MuzzleFlashSocket);

    UWorld* World = GetWorld();

    const FTransform& SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMesh);
    const FVector ToTarget = CachedHitTarget - SocketTransform.GetLocation();
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

    SpendRound();
}