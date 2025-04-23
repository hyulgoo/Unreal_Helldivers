
#include "Weapon/HDWeapon.h"
#include "Interface/HDWeaponInterface.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "HDCasing.h"
#include "Define/HDDefine.h"

AHDWeapon::AHDWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHDWeapon::OnSphereOverlap);

	ErgonomicFactor = 10.f;
}

void AHDWeapon::BeginPlay()
{
	Super::BeginPlay();

	Ammo = MaxAmmo;
}

void AHDWeapon::Fire(const FVector& HitTarget)
{
	NULL_CHECK(FireAnimation);
	WeaponMesh->PlayAnimation(FireAnimation, false);

	NULL_CHECK(CasingClass);
	const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
	if(AmmoEjectSocket)
	{
		FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
		UWorld* World = GetWorld();
		if(IsValid(World) && SocketTransform.IsValid())
		{				
			World->SpawnActor<AHDCasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
		}
	}

	SpendRound();
}

const FVector AHDWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to Found MuzzleFlash socket!"));
		return FVector();
	}

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMesh);
	const FVector Start				= SocketTransform.GetLocation();

	const FVector ToTarge			= HitTarget - Start;
	const float DistanceToSphere	= ToTarge.Size();
	const FVector SphereCenter		= Start + ToTarge.GetSafeNormal() * DistanceToSphere;

	UWorld* World = GetWorld();
	if (World)
	{
		//DrawDebugSphere(World, SphereCenter, SphereRadius, 12, FColor::Red, true);
		//DrawDebugSphere(World, HitTarget, SphereRadius, 12, FColor::Orange, true);
		//DrawDebugLine(World, Start, FVector(Start + ToTarge * DistanceToSphere), FColor::Cyan, true);
	}

	return FVector(Start + ToTarge * HITSCAN_TRACE_LENGTH);
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
