
#include "Character/HDCharacterPlayer.h"
#include "Define/HDDefine.h"
#include "Define/HDMontageSectionNames.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/Character/HDCombatComponent.h"
#include "Component/Character/HDInputActionComponent.h"
#include "Component/Character/HDStratagemComponent.h"
#include "Controller/HDPlayerController.h"
#include "Stratagem/HDStratagem.h"
#include "Animation/HDAnimInstance.h"
#include "Character/HDCharacterControlData.h"

AHDCharacterPlayer::AHDCharacterPlayer()
    : SpringArm(nullptr)
	, FollowCamera(nullptr)
	, Combat(nullptr)
    , InputAction(nullptr)
    , Stratagem(nullptr)
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // Camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    Combat      = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));
    InputAction = CreateDefaultSubobject<UHDInputActionComponent>(TEXT("InputAction"));
    Stratagem   = CreateDefaultSubobject<UHDStratagemComponent>(TEXT("Stratagem"));
}

void AHDCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    Combat->SpawnDefaultWeapon();
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    if (bRagdoll == false)
    {
        SetCharacterMovementState(EHDCharacterMovementState::Prone, true);

        FTimerDelegate LamdaDelegate;
        LamdaDelegate.BindLambda([this]() {
            SetCombatState(EHDCombatState::Unoccupied);
            });
        GetWorldTimerManager().SetTimerForNextTick(LamdaDelegate);
    }
}

void AHDCharacterPlayer::SetWeaponActive(const bool bActive)
{
    Combat->SetWeaponActive(bActive);

    UHDAnimInstance* HDCharacterInstance = Cast<UHDAnimInstance>(GetMesh()->GetAnimInstance());
    NULL_CHECK(HDCharacterInstance);

    HDCharacterInstance->SetUseUpperSlot(bActive == false);
}

const float AHDCharacterPlayer::Reload()
{
    if (Combat->CanReload() == false)
    {
        return 0.f;
    }

	Combat->Reload();

    const bool bIsShoulder = IsShouldering();
    const EHDCharacterMovementState MovementState = InputAction->GetCharacterMovementState();

	FName SectionName;
    switch (Combat->GetWeaponFireType())
	{
	case EHDFireType::HitScan:
	case EHDFireType::Projectile:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? HDMONTAGE_SECTIONNAME_RIFLE_PRONE
			: bIsShoulder ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
		break;
	case EHDFireType::Shotgun:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? HDMONTAGE_SECTIONNAME_SHOTGUN_PRONE
			: bIsShoulder ? HDMONTAGE_SECTIONNAME_SHOTGUN_AIM : HDMONTAGE_SECTIONNAME_SHOTGUN_HIP;
		break;
	}

	CONDITION_CHECK_WITH_RETURNTYPE(SectionName.IsNone(), 0.f);

	PlayMontage(ReloadWeaponMontage, SectionName);

    return Combat->GetWeaponReloadDelay(bIsShoulder);
}

void AHDCharacterPlayer::ReloadFinished()
{
    Combat->ReloadFinished();



    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK(PlayerController);

    PlayerController->ChangeCapacityHUDInfo(Combat->GetWeaponCapacityCount());
}

const float AHDCharacterPlayer::GetAimOffset_Yaw() const
{
    return Combat->GetAimOffset_Yaw();
}

const float AHDCharacterPlayer::GetAimOffset_Pitch() const
{
    return Combat->GetAimOffset_Pitch();
}

const bool AHDCharacterPlayer::IsShouldering() const
{
	return Combat->IsShoulder();
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    Combat->SetShoulder(bShoulder);   
}

const bool AHDCharacterPlayer::IsCharacterLookingViewport() const
{
    return Combat->IsCharacterLookingViewport();
}

const EHDTurningInPlace AHDCharacterPlayer::GetTurningInPlace() const
{
    return EHDTurningInPlace();
}

const bool AHDCharacterPlayer::IsUseRotateBone() const
{
    return Combat->IsUseRotateBone();
}

AHDWeapon* AHDCharacterPlayer::GetWeapon() const
{
    return Combat->GetWeapon();
}

const FVector& AHDCharacterPlayer::GetHitTarget() const
{
    return Combat->GetHitTarget();
}

const EHDCombatState AHDCharacterPlayer::GetCombatState() const
{
    return Combat->GetCombatState();
}

void AHDCharacterPlayer::SetCombatState(const EHDCombatState State)
{
    Combat->SetCombatState(State);
}

void AHDCharacterPlayer::Attack(const bool bActive)
{
    const EHDCombatState CombatState = GetCombatState();
    if (CombatState == EHDCombatState::Unoccupied)
    {
        Fire(bActive);
    }
    else if (CombatState == EHDCombatState::HoldStratagem)
    {
        NULL_CHECK(ThrowMontage);

        // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
        // 실제 AddImpulse는 AnimNotify에서 DetachTiming에 함
        PlayMontage(ThrowMontage);
    }
}

void AHDCharacterPlayer::SetCharacterControlData(UHDCharacterControlData* CharacterControlData)
{
    bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

    UCharacterMovementComponent* CharacterMovementComponent     = GetCharacterMovement();
    CharacterMovementComponent->bOrientRotationToMovement       = CharacterControlData->bOrientRotationToMovement;
    CharacterMovementComponent->bUseControllerDesiredRotation   = CharacterControlData->bUseControllerDesiredRotation;
    CharacterMovementComponent->RotationRate                    = CharacterControlData->RotationRate;

    SpringArm->TargetArmLength         = CharacterControlData->TargetArmLength;
    SpringArm->TargetOffset            = CharacterControlData->TargetOffset;
    SpringArm->SocketOffset            = CharacterControlData->SocketOffset;
    SpringArm->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
    SpringArm->bInheritPitch           = CharacterControlData->bInheritPitch;
    SpringArm->bInheritYaw             = CharacterControlData->bInheritYaw;
    SpringArm->bInheritRoll            = CharacterControlData->bInheritRoll;
    SpringArm->bDoCollisionTest        = CharacterControlData->bDoCollisionTest;

    Combat->SetSpringArmTargetLength(CharacterControlData->TargetArmLength);
    InputAction->SetSpringArmDefaultZOffset(CharacterControlData->TargetOffset.Z);
}

UHDStratagemComponent* AHDCharacterPlayer::GetStratagemComponent()
{
    return Stratagem;
}

const bool AHDCharacterPlayer::IsSprint() const
{
    return InputAction->IsSprint();
}

void AHDCharacterPlayer::SetSprint(const bool bSprint)
{
    // 해당 클래스를 상속받은 캐릭터에서 해당 함수 Override하여 스피드 조정 중
    InputAction->SetSprint(bSprint);
}

const EHDCharacterMovementState AHDCharacterPlayer::GetCharacterMovementState() const
{
    return InputAction->GetCharacterMovementState();
}

void AHDCharacterPlayer::SetCharacterMovementState(const EHDCharacterMovementState NewState, const bool bForced)
{
    InputAction->SetCharacterMovementState(NewState, bForced);
	if (bForced)
	{
        SetCombatState(NewState == EHDCharacterMovementState::Prone ? EHDCombatState::Ragdoll : EHDCombatState::Unoccupied);
    }
}

void AHDCharacterPlayer::RestoreMovementState()
{
    InputAction->RestoreMovementState();
    InputAction->ChangeCameraZOffsetByCharacterMovementState(InputAction->GetCharacterMovementState());
}

void AHDCharacterPlayer::DetachStratagemWhileThrow()
{
    Stratagem->ThrowFinished();
    SetCombatState(EHDCombatState::Unoccupied);
    SetWeaponActive(true);
}

void AHDCharacterPlayer::TryHoldStratagem()
{   
    const bool CanHoldStrategem = Stratagem->IsSelectedStratagemExist() && GetCombatState() != EHDCombatState::HoldStratagem;
    if (CanHoldStrategem)
    {
        Stratagem->HoldStratagem(GetMesh(), GetHitTarget());
        SetWeaponActive(false);

        SetCombatState(EHDCombatState::HoldStratagem);
    }
    else
    {
        CancleStratagem();
    }

    Stratagem->ClearCommand();
}

void AHDCharacterPlayer::CancleStratagem()
{
    if (Combat->GetCombatState() != EHDCombatState::HoldStratagem)
    {
        return;
    }

    Stratagem->CancleStratagem();
    SetCombatState(EHDCombatState::Unoccupied);
}

void AHDCharacterPlayer::InterpFOV(float DeltaSeconds)
{
    const bool bIsShoulder = IsShouldering();
    const float TargetFOV = bIsShoulder ? Combat->GetWeaponZoomedFOV() : Combat->GetDefaultFOV();
    const float InterpSpeed = bIsShoulder ? Combat->GetWeaponZoomInterpSpeed() : Combat->GetZoomInterpSpeed();
    const float NewInterpFOV = FMath::FInterpTo(Combat->GetCurrentFOV(), TargetFOV, DeltaSeconds, InterpSpeed);
    Combat->SetCurrentFOV(NewInterpFOV);
    
    FollowCamera->SetFieldOfView(Combat->GetCurrentFOV());
}

void AHDCharacterPlayer::PlayMontage(UAnimMontage* Montage, const FName SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    VALID_CHECK(AnimInstance);
    NULL_CHECK(Montage);

    AnimInstance->Montage_Play(Montage);

    if (SectionName.IsNone() == false)
    {
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

const float AHDCharacterPlayer::Fire(const bool IsPressed)
{
    if(Combat->Fire(IsPressed) == false)
    {
        if (Combat->NeedReload())
        {
            Reload();
        }

        return 0.f;
    }

    switch (Combat->GetWeaponFireType())
    {
    case EHDFireType::HitScan:
    case EHDFireType::Projectile:
    {
        const FName SectionName = IsShouldering() ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
        PlayMontage(FireWeaponMontage, SectionName);
    }
    break;
    case EHDFireType::Shotgun:
    {
        // TODO
        //const FName SectionName = IsShouldering() ? MONTAGESECTIONNAME_SHOTGUN_AIM : MONTAGESECTIONNAME_SHOTGUN_HIP;
        //PlayMontage(FireWeaponMontage);
        //Shotgun->FireShotgun(TraceHitTargets);
        //CombatState = ECombatState::ECS_Unoccupied;
    }
    break;
    }

    return Combat->GetWeaponFireDelay();
}

const bool AHDCharacterPlayer::FireFinished()
{
    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK_WITH_RETURNTYPE(PlayerController, false);

    PlayerController->ChangeAmmoHUDInfo(Combat->GetWeaponAmmoCount());

    return Combat->FireFinished();
}

void AHDCharacterPlayer::SetDead()
{
    Super::SetDead();

    APlayerController* PlayerController = GetController<APlayerController>();
    NULL_CHECK(PlayerController);

    if (IsLocallyControlled())
    {
        DisableInput(PlayerController);
    }
}
