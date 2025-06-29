// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDWeaponInterface.generated.h"

class AHDWeapon;
enum class EHDCombatState : uint8;

UINTERFACE(MinimalAPI)
class UHDWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDWeaponInterface
{
	GENERATED_BODY()

public:
	virtual AHDWeapon*				GetWeapon() const = 0;

	virtual const FVector&			GetHitTarget() const = 0;
	virtual const EHDCombatState	GetCombatState() const = 0;

	virtual void					Attack(const bool bActive) = 0;
	virtual const bool				FireFinished() = 0;

	virtual void					SetWeaponActive(const bool bActive) = 0;

	virtual const float				Reload() = 0;
	virtual void					ReloadFinished() = 0;
};
