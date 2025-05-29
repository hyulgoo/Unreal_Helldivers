
#include "Weapon/HDWeapon.h"
#include "HDCasing.h"
#include "Define/HDDefine.h"
#include "Interface/HDWeaponInterface.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"

AHDWeapon::AHDWeapon()
	: FireType(EFireType::Count)
	, CrosshaiTextureList{}
    , ZoomedFOV(30.f)
    , ZoomInterpSpeed(20.f)
    , ProjectileClass(nullptr)
    , WeaponType(EWeaponType::Count)
    , WeaponMesh(nullptr)
    , AreaSphere(nullptr)
    , FireAnimation(nullptr)
    , EquipSound(nullptr)
    , CasingClass(nullptr)
    , WeaponIconImage(nullptr)
    , FireDelay(0.f)
    , ErgonomicFactor(0.f)
    , Ammo(0.f)
    , MaxAmmo(0.f)
    , Capacity(0.f)
    , MaxCapacity(0.f)
    , bIsCanUseAutoFire(false)
    , bUseScatter(false)
    , bIsAutoFire(false)
    , WeaponState(EWeaponState::Drop)
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHDWeapon::OnSphereOverlap);
}

void AHDWeapon::BeginPlay()
{
	Super::BeginPlay();

	Ammo = MaxAmmo;
	Capacity = MaxCapacity;
}

void AHDWeapon::Fire(const FVector& HitTarget)
{
	NULL_CHECK(CasingClass);

	NULL_CHECK(FireAnimation);
	WeaponMesh->PlayAnimation(FireAnimation, false);

	const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
	NULL_CHECK(AmmoEjectSocket);

    FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
    CONDITION_CHECK(SocketTransform.IsValid() == false);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    World->SpawnActor<AHDCasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());

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

void AHDWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult)
{
	IHDWeaponInterface* WeaponInterface = Cast<IHDWeaponInterface>(OtherActor);
	if(WeaponInterface)
	{
		WeaponInterface->EquipWeapon(this);
	}
}

void AHDWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MaxAmmo);
}
