
#include "Weapon/HDWeapon.h"
#include "HDCasing.h"
#include "Define/HDDefine.h"
#include "Collision/HDCollision.h"
#include "Component/Character/HDCombatComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"

AHDWeapon::AHDWeapon()
	: FireType(EHDFireType::Count)
	, CrosshaiTextureList{}
    , ZoomedFOV(30.f)
    , ZoomInterpSpeed(20.f)
    , ProjectileClass(nullptr)
    , WeaponType(EWeaponType::Count)
    , WeaponMesh(nullptr)
    , AreaSphere(nullptr)
	, WeaponAnimationMap{}
    , EquipSound(nullptr)
    , CasingClass(nullptr)
    , WeaponIconImage(nullptr)
    , FireDelay(0.f)
    , ErgonomicFactor(0.f)
    , Ammo(0.f)
    , MaxAmmo(0.f)
    , Capacity(0.f)
    , MaxCapacity(0.f)
    , bUseScatter(false)
    , bIsAutoFire(false)
    , WeaponState(EWeaponState::Drop)
	, ReloadTimer(FTimerHandle())
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionProfileName(HDCOLLISION_PROFILE_TRIGGER);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHDWeapon::OnSphereOverlap);
}

void AHDWeapon::BeginPlay()
{
	Super::BeginPlay();

	Ammo = MaxAmmo;
	Capacity = MaxCapacity;
}

void AHDWeapon::Fire(const FVector& HitTarget, const bool bIsShoulder)
{
	CONDITION_CHECK(WeaponAnimationMap.Num() != static_cast<int32>(EHDWeaponAnimationType::Count));
	NULL_CHECK(CasingClass);

	UAnimationAsset* FireAnim = bIsShoulder ? WeaponAnimationMap[EHDWeaponAnimationType::Fire_Aim] : WeaponAnimationMap[EHDWeaponAnimationType::Fire_Hip];
	NULL_CHECK(FireAnim);
	WeaponMesh->PlayAnimation(FireAnim, false);

	const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
	NULL_CHECK(AmmoEjectSocket);

    const FTransform& SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
    CONDITION_CHECK(SocketTransform.IsValid() == false);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    World->SpawnActor<AHDCasing>(CasingClass, SocketTransform);

	SpendRound();
}

const void AHDWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	NULL_CHECK(WeaponMesh);

	const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
	NULL_CHECK(MuzzleFlashSocket);

	const FTransform& SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMesh);
	CONDITION_CHECK(SocketTransform.IsValid() == false);

	const FVector& Start		 = SocketTransform.GetLocation();
	const FVector ToTarge		 = HitTarget - Start;
	const float DistanceToSphere = ToTarge.Size();
	const FVector SphereCenter	 = Start + ToTarge.GetSafeNormal() * DistanceToSphere;
	
	UWorld* World = GetWorld();
	NULL_CHECK(World);

	DrawDebugSphere(World, HitTarget, 15.f, 12, FColor::Orange, false, 0.1f);
}

const bool AHDWeapon::IsAmmoEmpty() const
{
	return Ammo <= 0;
}

const bool AHDWeapon::IsAmmoFull() const
{
	return Ammo == MaxAmmo;
}

const bool AHDWeapon::IsCapacityEmpty() const
{
	return Capacity <= 0;
}

const bool AHDWeapon::IsCapacityFull() const
{
	return Capacity == MaxCapacity;
}

FORCEINLINE const float AHDWeapon::GetReloadDelay(const bool bIsShoulder) const
{
	CONDITION_CHECK_WITH_RETURNTYPE(WeaponAnimationMap.Num() != static_cast<int32>(EHDWeaponAnimationType::Count), 0.f);
	UAnimationAsset* ReloadAnim = bIsShoulder ? WeaponAnimationMap[EHDWeaponAnimationType::Reload_Aim] : WeaponAnimationMap[EHDWeaponAnimationType::Reload_Hip];
	NULL_CHECK_WITH_RETURNTYPE(ReloadAnim, 0.f);
	return ReloadAnim->GetPlayLength();
}

void AHDWeapon::Reload(const bool bIsShoulder)
{
	CONDITION_CHECK(WeaponAnimationMap.Num() != static_cast<int32>(EHDWeaponAnimationType::Count));

	UAnimationAsset* ReloadAnim = bIsShoulder ? WeaponAnimationMap[EHDWeaponAnimationType::Reload_Aim] : WeaponAnimationMap[EHDWeaponAnimationType::Reload_Hip];
	NULL_CHECK(ReloadAnim);

	WeaponMesh->PlayAnimation(ReloadAnim, false);
	
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &AHDWeapon::ReloadFinished, GetReloadDelay(bIsShoulder));
}

void AHDWeapon::AddCapacity(const int32 NewCapacityCount)
{
	Capacity = FMath::Clamp(Capacity + NewCapacityCount, 0, MaxCapacity);
}

void AHDWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult)
{
	UHDCombatComponent* Combat = OtherActor->GetComponentByClass<UHDCombatComponent>();
	if(Combat)
	{
		Combat->EquipWeapon(this);
	}
}

void AHDWeapon::SpendRound()
{
	CONDITION_CHECK(WeaponState != EWeaponState::Equip);

	Ammo = FMath::Clamp(Ammo - 1, 0, MaxAmmo);
}

void AHDWeapon::ReloadFinished()
{
	CONDITION_CHECK(WeaponState != EWeaponState::Equip);

	Capacity = FMath::Clamp(Capacity - 1, 0, MaxCapacity);

	Ammo = MaxAmmo;
}
