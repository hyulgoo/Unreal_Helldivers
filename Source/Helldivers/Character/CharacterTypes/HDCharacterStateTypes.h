#pragma once

UENUM(BlueprintType)
enum class EHDArmorType : uint8
{
	Light,
    Medium,
    Heavy,
    Count
};

UENUM(BlueprintType)
enum class EHDCharacterControlType : uint8
{
	FirstPerson,
	ThirdPerson,
};

UENUM(BlueprintType)
enum class EHDCombatState : uint8
{
	Unoccupied,
    Fire,
	Reloading,
	SwappingWeapons,
	HoldStratagem,
	Throwing,
	Ragdoll,
	Count
};

UENUM(BlueprintType)
enum class EHDCommandInput : uint8
{
    Up,
    Down,
    Left,
    Right,
    Count,
};

UENUM(BlueprintType)
enum class EHDTurningInPlace : uint8
{
	NotTurning,
	Turn_Left,
	Turn_Right,
	Count,
};

UENUM(BlueprintType)
enum class EHDCharacterStanceState : uint8
{
	Idle,
	Crouch,
	Prone,
	Count,
};

UENUM(BlueprintType)
enum class EHDCharacterMovementState : uint8
{
	Idle,
    Walk,
    Sprint,
	Count,
};

UENUM(BlueprintType)
enum class EHDCombatMontage : uint8
{
	Fire,
    Reload,
    Throw,
	Count,
};

UENUM(BlueprintType)
enum class EHDCharacterType: uint8
{
	Player,
    NPC,
};