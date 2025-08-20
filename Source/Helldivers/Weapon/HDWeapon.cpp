
#include "Weapon/HDWeapon.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"
#include "Collision/HDCollision.h"
#include "Interface/HDWeaponInterface.h"
#include "Projectile/HDProjectileBase.h"

AHDWeapon::AHDWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionProfileName(HDCOLLISION_PROFILE_TRIGGER);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHDWeapon::OnSphereOverlap);
}

void AHDWeapon::EquipWeapon(AActor* OwnerActor)
{
    NULL_CHECK(OwnerActor);

    SetOwner(OwnerActor);
    WeaponState = EWeaponState::Equip;
}

const void AHDWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	NULL_CHECK(WeaponMesh);

	const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMesh->GetSocketByName(HDSOCKETNAME_MUZZLEFLASH);
	NULL_CHECK(MuzzleFlashSocket);

	const FVector& Start		 = MuzzleFlashSocket->GetSocketLocation(WeaponMesh);
	const FVector ToTarge		 = HitTarget - Start;
	const float DistanceToSphere = ToTarge.Size();
	const FVector SphereCenter	 = Start + ToTarge.GetSafeNormal() * DistanceToSphere;

	DrawDebugSphere(GetWorld(), HitTarget, 15.f, 12, FColor::Orange, false, 0.1f);
}

void AHDWeapon::SpawnProjectile(const FVector& HitTarget)
{
    UWorld* World = GetWorld();

    const FTransform& MuzzleSocketTransform = WeaponMesh->GetSocketTransform(HDSOCKETNAME_MUZZLEFLASH);
    const FVector ToTarget = HitTarget - MuzzleSocketTransform.GetLocation();
    const FTransform SpawnTransform(ToTarget.Rotation(), MuzzleSocketTransform.GetLocation());

    AHDProjectileBase* SpawnedProjectile = World->SpawnActor<AHDProjectileBase>(ProjectileClass, SpawnTransform);
    NULL_CHECK(SpawnedProjectile); 
    SpawnedProjectile->SetOwner(GetOwner());

    const FTransform& EjectSocketTransform = WeaponMesh->GetSocketTransform(HDSOCKETNAME_AMMOEJECT);
    CONDITION_CHECK(EjectSocketTransform.IsValid());

    World->SpawnActor<AHDCasing>(CasingClass, EjectSocketTransform);
}

void AHDWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult)
{
    TScriptInterface<IHDWeaponInterface> WeaponInterface = OtherActor;
	if(WeaponInterface)
	{
        WeaponInterface->EquipWeapon(this);
	}
}