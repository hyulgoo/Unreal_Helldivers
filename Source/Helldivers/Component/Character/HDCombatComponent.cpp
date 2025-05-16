
#include "Component/Character/HDCombatComponent.h"
#include "Define/HDDefine.h"
#include "Weapon/HDWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Animation/HDAnimInstance.h"

UHDCombatComponent::UHDCombatComponent()
: bIsShoulder(false)
, bCanFire(false)
, bIsFireButtonPressed(false)
, FireTimer{}
, CombatState(EHDCombatState::Count)
, CurrentFOV(0.f)
, ErgonomicFactor(0.f)
{
	bCanFire = true;
	bIsFireButtonPressed = false;
}

void UHDCombatComponent::FireTimerFinished()
{
    if (IsValid(Weapon) == false)
    {
        return;
    }

    bCanFire = true;
    if (bIsFireButtonPressed && Weapon->IsAutoFire())
    {
        Fire(true);
    }

    // TODO Reload
   /* AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);
    PlayerController->ChangeCapacityHUDInfo(Weapon->GetCapacityCount());*/
}

const bool UHDCombatComponent::Fire(const bool IsPressed)
{
    bIsFireButtonPressed = IsPressed;

    if (bIsFireButtonPressed == false || CanFire() == false)
    {
        return false;
    }

    if (CombatState == EHDCombatState::Unoccupied)
    {
        Weapon->Fire(HitTarget);
        
        GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &UHDCombatComponent::FireTimerFinished, Weapon->GetFireDelay()) ;
        return true;
    }

    return false;
}

void UHDCombatComponent::EquipWeapon(AHDWeapon* NewWeapon)
{
    VALID_CHECK(NewWeapon);

    Weapon = NewWeapon;
    Weapon->SetWeaponState(EWeaponState::Equip);
    ErgonomicFactor = Weapon->GetErgonomicFactor();
}

const bool UHDCombatComponent::CanFire()
{
    if (IsValid(Weapon) == false || bCanFire == false || Weapon->IsAmmoEmpty())
    {
        return false;
    }

    if (Weapon->GetWeaponType() == EWeaponType::Shotgun && CombatState == EHDCombatState::Reloading)
    {
        return true;
    }

    return (bCanFire && CombatState == EHDCombatState::Unoccupied);
}
