
#include "Component/Character/HDCombatComponent.h"
#include "Define/HDDefine.h"
#include "Weapon/HDWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/HDAnimInstance.h"

#define AIMOFFSET_PITCH_OFFSET 20.f

UHDCombatComponent::UHDCombatComponent()
    : StartingAimRotation(FRotator())
    , AimOffset_Yaw(0.f)
    , InterpAimOffset_Yaw(0.f)
    , AimOffset_Pitch(0.f)
    , bIsCharacterLookingViewport(false)
    , bUseRotateRootBone(false)
    , TurnThreshold(0.f)
    , TurningInPlace(EHDTurningInPlace::NotTurning)
    , bIsShoulder(false)
    , bIsFireButtonPressed(false)
    , CombatState(EHDCombatState::Unoccupied)
    , HitTarget(FVector())
    , DefaultFOV(50.f)
	, ZoomedFOV(0.f)
	, CurrentFOV(0.f)
	, ZoomInterpSpeed(0.f)
    , ErgonomicFactor(0.f)
	, Weapon(nullptr)
    , DefaultCurve(nullptr)
    , SpringArmArmLengthTimeline(FTimeline())
    , SpringArmTargetArmLength(0.f)
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHDCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    CharacterMovement = GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
    check(CharacterMovement);

    SpringArm = GetOwner()->GetComponentByClass<USpringArmComponent>();
    check(SpringArm);

    FOnTimelineFloat ArmLengthTimelineProgress;
    ArmLengthTimelineProgress.BindUFunction(this, FName("OnCameraSpringArmLengthTimelineUpdate"));
    SpringArmArmLengthTimeline.AddInterpFloat(DefaultCurve, ArmLengthTimelineProgress);
    SpringArmArmLengthTimeline.SetLooping(false);
}

void UHDCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TraceUnderCrosshairs();
    SpringArmArmLengthTimeline.TickTimeline(DeltaTime);
}

const bool UHDCombatComponent::FireFinished()
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, false);

    if (bIsFireButtonPressed && Weapon->IsAutoFire())
    {
        Fire(true);
        return true;
    }
    return false;
}

void UHDCombatComponent::ReloadFinished()
{
    CombatState = EHDCombatState::Unoccupied;
}

const float UHDCombatComponent::GetAimOffset_Yaw() const
{
    return AimOffset_Yaw;
}

const float UHDCombatComponent::GetAimOffset_Pitch() const
{
    return AimOffset_Pitch;
}

const FVector& UHDCombatComponent::GetHitTarget() const
{
    return HitTarget;
}

void UHDCombatComponent::SetHitTarget(const FVector& NewHitTarget)
{
    HitTarget = NewHitTarget;
}

const float UHDCombatComponent::GetCurrentFOV() const
{
    return CurrentFOV;
}

void UHDCombatComponent::SetCurrentFOV(const float NewFOV)
{
    CurrentFOV = NewFOV;
}

const float UHDCombatComponent::GetZoomInterpSpeed() const
{
    return ZoomInterpSpeed;
}

const bool UHDCombatComponent::IsCharacterLookingViewport() const
{
    return bIsCharacterLookingViewport;
}

void UHDCombatComponent::SetSpringArmTargetLength(const float TargetArmLength)
{
    SpringArmTargetArmLength = TargetArmLength;
}

const float UHDCombatComponent::GetDefaultFOV() const
{
    return DefaultFOV;
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

void UHDCombatComponent::AimOffset(const float DeltaTime)
{
    NULL_CHECK(CharacterMovement);

    if (IsValid(Weapon) == false)
    {
        return;
    }

    AActor* Owner = GetOwner();
    APawn* OwnerPawn = Cast<APawn>(Owner);

    FVector Velocity = Owner->GetVelocity();
    Velocity.Z = 0.f;
    const bool bIsMoving = Velocity.Size() > 0.1f;
    const bool bIsFalling = CharacterMovement->IsFalling();
    const FRotator BaseAimRoatation = OwnerPawn->GetBaseAimRotation();
    const FRotator ControlRotation = OwnerPawn->GetControlRotation();

    if (bIsShoulder)
    {
        bIsCharacterLookingViewport = true;
        bUseRotateRootBone = false;
        OwnerPawn->bUseControllerRotationYaw = true;
        CharacterMovement->bOrientRotationToMovement = false;

        const FRotator TargetRotation(0.f, ControlRotation.Yaw, 0.f);
        const FRotator SmoothRotation = FMath::RInterpTo(Owner->GetActorRotation(), TargetRotation, DeltaTime, 20.f);
        Owner->SetActorRotation(SmoothRotation);

        if (bIsMoving == false && bIsFalling == false)
        {
            const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, StartingAimRotation);
            AimOffset_Yaw = DeltaRotation.Yaw;

            if (TurningInPlace == EHDTurningInPlace::NotTurning)
            {
                InterpAimOffset_Yaw = AimOffset_Yaw;
            }

            TurnInPlace(DeltaTime);
        }

        if (bIsMoving || bIsFalling)
        {
            AimOffset_Yaw = 0.f;
            InterpAimOffset_Yaw = 0.f;
            StartingAimRotation = TargetRotation;
        }
    }
    else
    {
        bIsCharacterLookingViewport = true;
        bUseRotateRootBone = true;
        OwnerPawn->bUseControllerRotationYaw = false;
        CharacterMovement->bOrientRotationToMovement = false;
        const FRotator TargetRotation(0.f, BaseAimRoatation.Yaw, 0.f);
        const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, StartingAimRotation);
        AimOffset_Yaw = DeltaRotation.Yaw;

        if (bIsMoving == false && bIsFalling == false)
        {
            if (TurningInPlace == EHDTurningInPlace::NotTurning)
            {
                InterpAimOffset_Yaw = AimOffset_Yaw;
            }

            bUseRotateRootBone = false;
            OwnerPawn->bUseControllerRotationYaw = true;
            TurnInPlace(DeltaTime);
        }

        if (bIsMoving || bIsFalling)
        {
            bIsCharacterLookingViewport = false;
            bUseRotateRootBone = false;
            AimOffset_Yaw = 0.f;
            StartingAimRotation = BaseAimRoatation;
            CharacterMovement->bOrientRotationToMovement = true;
        }
    }

    AimOffset_Pitch = BaseAimRoatation.Pitch - AIMOFFSET_PITCH_OFFSET;
}

void UHDCombatComponent::TurnInPlace(const float DeltaTime)
{
    if (AimOffset_Yaw > TurnThreshold)
    {
        TurningInPlace = EHDTurningInPlace::Turn_Right;
    }
    else if (AimOffset_Yaw < -TurnThreshold)
    {
        TurningInPlace = EHDTurningInPlace::Turn_Left;
    }

    if (TurningInPlace != EHDTurningInPlace::NotTurning)
    {
        InterpAimOffset_Yaw = FMath::FInterpTo(InterpAimOffset_Yaw, 0.f, DeltaTime, 4.f);
        AimOffset_Yaw = InterpAimOffset_Yaw;
        if (FMath::Abs(AimOffset_Yaw) < 15.f)
        {
            StartingAimRotation = FRotator(0.f, GetOwner<APawn>()->GetBaseAimRotation().Yaw, 0.f);
            TurningInPlace = EHDTurningInPlace::NotTurning;
        }
    }
}

void UHDCombatComponent::OnSpringArmLengthUpdate(const float Value)
{
    const float Interpolated = FMath::Lerp(SpringArmTargetArmLength, SpringArmTargetArmLength / 2.f, Value);
    SpringArm->TargetArmLength = Interpolated;
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

AHDWeapon* UHDCombatComponent::GetWeapon() const
{
    return Weapon;
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

const EHDCombatState UHDCombatComponent::GetCombatState() const
{
    return CombatState;
}

void UHDCombatComponent::SetCombatState(const EHDCombatState State)
{
    CONDITION_CHECK(CombatState == State);

    CombatState = State;
}

const bool UHDCombatComponent::IsShoulder() const
{
    return bIsShoulder;
}

void UHDCombatComponent::SetShoulder(const bool bShoudler)
{
    bIsShoulder = bShoudler;

	if (bIsShoulder)
	{
		SpringArmArmLengthTimeline.PlayFromStart();
	}
	else
	{
		SpringArmArmLengthTimeline.ReverseFromEnd();
	}
}

const bool UHDCombatComponent::CanReload() const
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
