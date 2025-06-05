#pragma once

#define HITSCAN_TRACE_LENGTH 20000.f
#define EXPECTED_IMPACT_POINT_TRACE_LENGTH 1000.f;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Init,
	Equip,
    Drop,
	Count,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AssaultRifle,
	RocketLauncher,
	Pistol,
	SubmachineGun,
	Shotgun,
	SniperRifle,
	GrenadeLauncher,
	Count,
};

UENUM(BlueprintType)
enum class EHDFireType : uint8
{
	HitScan,
	Projectile,
	Shotgun,
	Count,
};

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
	None,
	Fire,
	Poison,
	Sturn,
	Count,
};

UENUM(BlueprintType)
enum class EImpactType : uint8
{
	Hit,
	Explode,
	Penetrate
};