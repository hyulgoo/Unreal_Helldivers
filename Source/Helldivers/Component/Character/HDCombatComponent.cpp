
#include "Component/Character/HDCombatComponent.h"
#include "Define/HDDefine.h"
#include "Weapon/HDWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CharacterTypes/HDCombatState.h"
#include "Animation/HDAnimInstance.h"

UHDCombatComponent::UHDCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bCanFire = true;
	bIsFireButtonPressed = false;
}


void UHDCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
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

void UHDCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
        FireTimerFinished();
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
