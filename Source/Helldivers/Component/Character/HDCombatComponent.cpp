
#include "Component/Character/HDCombatComponent.h"
#include "Define/HDDefine.h"
#include "Weapon/HDWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/HDAnimInstance.h"

UHDCombatComponent::UHDCombatComponent()
    : bIsShoulder(false)
    , bIsFireButtonPressed(false)
    , CombatState(EHDCombatState::Unoccupied)
    , HitTarget(FVector())
	, ZoomedFOV(0.f)
	, CurrentFOV(0.f)
	, ZoomInterpSpeed(0.f)
    , ErgonomicFactor(0.f)
	, Weapon(nullptr)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    SetComponentTickEnabled(true);
}

void UHDCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TraceUnderCrosshairs();
}

const bool UHDCombatComponent::FireTimerFinished()
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, false);

    if (bIsFireButtonPressed && Weapon->IsAutoFire())
    {
        Fire(true);
        return true;
    }

    return false;
}

void UHDCombatComponent::ReloadTimerFinished()
{
    CombatState = EHDCombatState::Unoccupied;
}

void UHDCombatComponent::TraceUnderCrosshairs()
{
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    else
    {
        CONDITION_CHECK(true);
    }

    const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;
    const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection
    );

    if (bScreenToWorld)
    {
        FVector Start = CrosshairWorldPosition;
        const float DistanceToCharacter = (GetOwner()->GetActorLocation() - Start).Size();
        Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);

        const FVector End = Start + CrosshairWorldDirection * HITSCAN_TRACE_LENGTH;
        UWorld* World = GetWorld();
        VALID_CHECK(World);

        FHitResult TraceHitResult;
        const bool bHit = World->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);        

        HitTarget = bHit ? TraceHitResult.ImpactPoint : End;

        NULL_CHECK(Weapon);
        Weapon->TraceEndWithScatter(HitTarget);
    }
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
        Weapon->Fire(HitTarget, bIsShoulder);
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
    if (IsValid(Weapon) == false || Weapon->IsAmmoEmpty())
    {
        return false;
    }

    if (Weapon->GetWeaponType() == EWeaponType::Shotgun && CombatState == EHDCombatState::Reloading)
    {
        return true;
    }

    return (CombatState == EHDCombatState::Unoccupied);
}

const bool UHDCombatComponent::CanReload()
{
    if (IsValid(Weapon) == false || Weapon->IsCapacityEmpty() || Weapon->IsAmmoFull())
    {
        return false;
    }

    return CombatState == EHDCombatState::Unoccupied;
}

void UHDCombatComponent::Reload()
{
    NULL_CHECK(Weapon);

    CombatState = EHDCombatState::Reloading;
    Weapon->Reload(bIsShoulder);
}
