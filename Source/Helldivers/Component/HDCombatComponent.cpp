
#include "HDCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"
#include "Weapon/HDWeapon.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

#define AIMOFFSET_PITCH_OFFSET 20.f

UHDCombatComponent::UHDCombatComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
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
    ArmLengthTimelineProgress.BindUFunction(this, FName("OnSpringArmLengthUpdate"));
    SpringArmArmLengthTimeline.AddInterpFloat(DefaultCurve, ArmLengthTimelineProgress);
    SpringArmArmLengthTimeline.SetLooping(false);
}

void UHDCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TraceUnderCrosshairs();
    AimOffset(DeltaTime);
    SpringArmArmLengthTimeline.TickTimeline(DeltaTime);
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

const EHDTurningInPlace UHDCombatComponent::GetTurnInPlace() const
{
    return TurningInPlace;
}

const float UHDCombatComponent::GetDefaultFOV() const
{
    return DefaultFOV;
}

const bool UHDCombatComponent::IsUseRotateBone() const
{
    return bUseRotateRootBone;
}

UAnimMontage* UHDCombatComponent::GetCombatMontage(const EHDCombatMontage MontageType)
{
    CONDITION_CHECK_WITH_RETURNTYPE(CombatMontage.Num() == static_cast<int32>(EHDCombatMontage::Count), nullptr);

    return CombatMontage[MontageType];
}

void UHDCombatComponent::TraceUnderCrosshairs()
{
    CONDITION_CHECK(GEngine && GEngine->GameViewport);

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);

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
    VALID_CHECK_WITHOUT_LOG(Weapon);

	APawn* Owner = GetOwner<APawn>();
    NULL_CHECK(Owner);

    FVector Velocity = Owner->GetVelocity();
    Velocity.Z = 0.f;
    const bool bIsMoving = Velocity.Size() > 0.1f;
    const bool bIsFalling = CharacterMovement->IsFalling();
    const FRotator BaseAimRoatation = Owner->GetBaseAimRotation();
    const FRotator ControlRotation = Owner->GetControlRotation();

    if (bIsShoulder)
    {
        bIsCharacterLookingViewport = true;
        bUseRotateRootBone = false;
        Owner->bUseControllerRotationYaw = true;
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
        Owner->bUseControllerRotationYaw = false;
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
            Owner->bUseControllerRotationYaw = true;
            TurnInPlace(DeltaTime);
        }

        if (bIsMoving || bIsFalling)
        {
            bIsCharacterLookingViewport = false;
            bUseRotateRootBone = false;
            AimOffset_Yaw = 0.f;
            StartingAimRotation = BaseAimRoatation;
            TurningInPlace = EHDTurningInPlace::NotTurning;
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

void UHDCombatComponent::EquipWeapon(AHDWeapon* NewWeapon, UAbilitySystemComponent* ASC)
{
    VALID_CHECK(NewWeapon);

    Weapon = NewWeapon;
    Weapon->EquipWeapon(GetOwner());
    ErgonomicFactor = Weapon->ErgoData.ErgonomicFactor;
    ZoomedFOV = Weapon->ErgoData.ZoomedFOV;
    ZoomInterpSpeed = Weapon->ErgoData.ZoomInterpSpeed;
    bIsEquipped = true;

    USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
    NULL_CHECK(SkeletalMesh);

    const USkeletalMeshSocket* RightHandSocket = SkeletalMesh->GetSocketByName(HDSOCKETNAME_RIGRHTHAND);
    NULL_CHECK(RightHandSocket);
    RightHandSocket->AttachActor(NewWeapon, SkeletalMesh);
}

void UHDCombatComponent::SpawnProjectile()
{
    VALID_CHECK(Weapon);

    Weapon->SpawnProjectile(HitTarget);
}

USkeletalMeshComponent* UHDCombatComponent::GetWeaponMesh() const
{
    VALID_CHECK_WITH_RETURNTYPE_WITHOUT_LOG(Weapon, nullptr);

    return Weapon->GetComponentByClass<USkeletalMeshComponent>();;
}

void UHDCombatComponent::SetWeaponActive(const bool bActive)
{
    VALID_CHECK(Weapon);
    Weapon->SetActorTickEnabled(bActive);
    Weapon->SetActorHiddenInGame(bActive == false);
    Weapon->SetActorEnableCollision(bActive);
}

UAnimMontage* UHDCombatComponent::GetWeaponMontage() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, nullptr);

    return Weapon->WeaponAnimMontage;
}

const EHDFireType UHDCombatComponent::GetWeaponFireType() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, EHDFireType::Count);

    return Weapon->FireType;
}

const float UHDCombatComponent::GetWeaponFireDelay() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    return Weapon->FireDelay;
}

const int32 UHDCombatComponent::GetWeaponAmmoCount() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0);

    return Weapon->AmmoData.Ammo;
}

const int32 UHDCombatComponent::GetWeaponMaxAmmoCount() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0);

    return Weapon->AmmoData.MaxAmmo;
}

const int32 UHDCombatComponent::GetWeaponCapacityCount() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0);

    return Weapon->AmmoData.Capacity;
}

const int32 UHDCombatComponent::GetWeaponMaxCapacityCount() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0);

    return Weapon->AmmoData.MaxCapacity;
}

const float UHDCombatComponent::GetWeaponZoomedFOV() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    return Weapon->ErgoData.ZoomedFOV;
}

const float UHDCombatComponent::GetWeaponZoomInterpSpeed() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    return Weapon->ErgoData.ZoomInterpSpeed;
}

const bool UHDCombatComponent::IsWeaponAutoFire() const
{
    VALID_CHECK_WITH_RETURNTYPE(Weapon, false);

    return Weapon->bIsAutoFire;
}

const bool UHDCombatComponent::IsEquippedWeapon() const
{
    return bIsEquipped;
}

AHDWeapon* UHDCombatComponent::SpawnDefaultWeapon()
{
    NULL_CHECK_WITH_RETURNTYPE(DefaultWeaponClass, nullptr);
    return GetWorld()->SpawnActor<AHDWeapon>(DefaultWeaponClass);
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

AHDWeapon* UHDCombatComponent::GetWeapon() const
{
    return Weapon;
}
