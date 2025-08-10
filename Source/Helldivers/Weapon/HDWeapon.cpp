
#include "Weapon/HDWeapon.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AbilitySystemComponent.h"
#include "HDCasing.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"
#include "Define/HDGameplayTag.h"
#include "Collision/HDCollision.h"
#include "Interface/HDWeaponInterface.h"

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

void AHDWeapon::SetAutoFire(const bool bAutoFire)
{
	bIsAutoFire = bAutoFire;
}

void AHDWeapon::SetWeaponState(const EWeaponState NewState)
{
	CONDITION_CHECK(NewState != EWeaponState::Count);
	WeaponState = NewState;
}

void AHDWeapon::BeginPlay()
{
	Super::BeginPlay();

    AmmoData.Ammo = AmmoData.MaxAmmo;
    AmmoData.Capacity = AmmoData.MaxCapacity;
}

USkeletalMeshComponent* AHDWeapon::GetWeaponMesh() const
{
	return WeaponMesh;
}

USphereComponent* AHDWeapon::GetAreaSphere() const
{
	return AreaSphere;
}

void AHDWeapon::EquipWeapon(AActor* OwnerActor, UAbilitySystemComponent* ASC)
{
    NULL_CHECK(OwnerActor);
    NULL_CHECK(ASC);

    SetOwner(OwnerActor);
    SetWeaponState(EWeaponState::Equip);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_SPAWN_PROJECTILE).AddUObject(this, &AHDWeapon::SpawnProjectile);
}

void AHDWeapon::Fire(const FVector& HitTarget, const bool bIsShoulder)
{
	CONDITION_CHECK(WeaponAnimationMap.Num() == static_cast<int32>(EHDWeaponAnimationType::Count));
	NULL_CHECK(CasingClass);

	UAnimationAsset* FireAnim = bIsShoulder ? WeaponAnimationMap[EHDWeaponAnimationType::Fire_Aim] : WeaponAnimationMap[EHDWeaponAnimationType::Fire_Hip];
	NULL_CHECK(FireAnim);

	WeaponMesh->PlayAnimation(FireAnim, false);

	const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(HDSOCKETNAME_AMMOEJECT);
	NULL_CHECK(AmmoEjectSocket);

    const FTransform& SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
    CONDITION_CHECK(SocketTransform.IsValid());

    GetWorld()->SpawnActor<AHDCasing>(CasingClass, SocketTransform);

    CachedHitTarget = HitTarget;
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

const EWeaponType AHDWeapon::GetWeaponType() const
{
    return WeaponType;
}

const EHDFireType AHDWeapon::GetFireType() const
{
    return FireType;
}

const bool AHDWeapon::IsAmmoEmpty() const
{
	return AmmoData.Ammo <= 0;
}

const bool AHDWeapon::IsAmmoFull() const
{
	return AmmoData.Ammo == AmmoData.MaxAmmo;
}

const bool AHDWeapon::IsCapacityEmpty() const
{
	return AmmoData.Capacity <= 0;
}

const bool AHDWeapon::IsCapacityFull() const
{
	return AmmoData.Capacity == AmmoData.MaxCapacity;
}

const bool AHDWeapon::IsUseScatter() const
{
	return bUseScatter;
}

const bool AHDWeapon::IsAutoFire() const
{
	return bIsAutoFire;
}

const float AHDWeapon::GetFireDelay() const
{
	return FireDelay;
}

const float AHDWeapon::GetErgonomicFactor() const
{
	return ErgoData.ErgonomicFactor;
}

const int32 AHDWeapon::GetAmmoCount() const
{
	return AmmoData.Ammo;
}

const int32 AHDWeapon::GetMaxAmmoCount() const
{
	return AmmoData.MaxAmmo;
}

const int32 AHDWeapon::GetCapacityCount() const
{
	return AmmoData.Capacity;
}

const int32 AHDWeapon::GetMaxCapacityCount() const
{
	return AmmoData.MaxCapacity;
}

const float AHDWeapon::GetReloadDelay(const bool bIsShoulder) const
{
	CONDITION_CHECK_WITH_RETURNTYPE(WeaponAnimationMap.Num() == static_cast<int32>(EHDWeaponAnimationType::Count), 0.f);

    const EHDWeaponAnimationType ReloadAnimType = bIsShoulder ? EHDWeaponAnimationType::Reload_Aim : EHDWeaponAnimationType::Reload_Hip;
    CONDITION_CHECK_WITH_RETURNTYPE(WeaponAnimationMap.Contains(ReloadAnimType), 0.f);

    UAnimationAsset* ReloadAnim = WeaponAnimationMap[ReloadAnimType];
	NULL_CHECK_WITH_RETURNTYPE(ReloadAnim, 0.f);

	return ReloadAnim->GetPlayLength();
}

UTexture2D* AHDWeapon::GetWeaponIconImage() const
{
	return UIData.WeaponIconImage;
}

const float AHDWeapon::GetZoomedFOV() const
{
	return ErgoData.ZoomedFOV;
}

const float AHDWeapon::GetZoomInterpSpeed() const
{
	return ErgoData.ZoomInterpSpeed;
}

void AHDWeapon::Reload(const bool bIsShoulder)
{
	CONDITION_CHECK(WeaponAnimationMap.Num() == static_cast<int32>(EHDWeaponAnimationType::Count));

    const EHDWeaponAnimationType ReloadAnimType = bIsShoulder ? EHDWeaponAnimationType::Reload_Aim : EHDWeaponAnimationType::Reload_Hip;
    CONDITION_CHECK(WeaponAnimationMap.Contains(ReloadAnimType));

    UAnimationAsset* ReloadAnim = WeaponAnimationMap[ReloadAnimType];
    NULL_CHECK(ReloadAnim);

	WeaponMesh->PlayAnimation(ReloadAnim, false);
	
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &AHDWeapon::ReloadFinished, GetReloadDelay(bIsShoulder));
}

void AHDWeapon::AddCapacity(const int32 NewCapacityCount)
{
	AmmoData.Capacity = FMath::Clamp(AmmoData.Capacity + NewCapacityCount, 0, AmmoData.MaxCapacity);
}

void AHDWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult)
{
    TScriptInterface<IHDWeaponInterface> WeaponInterface = OtherActor;
	if(WeaponInterface)
	{
        WeaponInterface->EquipWeapon(this);
	}
}

void AHDWeapon::SpendRound()
{
	CONDITION_CHECK(WeaponState== EWeaponState::Equip);

    AmmoData.Ammo = FMath::Clamp(AmmoData.Ammo - 1, 0, AmmoData.MaxAmmo);
}

void AHDWeapon::ReloadFinished()
{
	CONDITION_CHECK(WeaponState == EWeaponState::Equip);

    AmmoData.Capacity = FMath::Clamp(AmmoData.Capacity - 1, 0, AmmoData.MaxCapacity);

    AmmoData.Ammo = AmmoData.MaxAmmo;
}
