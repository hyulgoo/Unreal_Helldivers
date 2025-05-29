
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
    , bCanFire(false)
    , bIsFireButtonPressed(false)
    , FireTimer(FTimerHandle())
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

	bCanFire = true;
	bIsFireButtonPressed = false;
}

void UHDCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TraceUnderCrosshairs();
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

void UHDCombatComponent::TraceUnderCrosshairs()
{
    FVector2D ViewportSize = FVector2D();
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
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
        FHitResult TraceHitResult;

        FVector Start = CrosshairWorldPosition;
        const float DistanceToCharacter = (GetOwner()->GetActorLocation() - Start).Size();
        Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);

        const FVector End = Start + CrosshairWorldDirection * HITSCAN_TRACE_LENGTH;
        UWorld* World = GetWorld();
        VALID_CHECK(World);
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
