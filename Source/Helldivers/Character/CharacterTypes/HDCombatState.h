#pragma once

UENUM(BlueprintType)
enum class EHDCombatState : uint8
{
	Unoccupied,
	Reloading,
	ThrowingGrenade,
	SwappingWeapons,
	Count
};
