
#include "Character/HDCharacterPlayer.h"
#include "Define/HDDefine.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Component/Character/HDCombatComponent.h"
#include "Component/Character/HDInputActionComponent.h"
#include "Component/Character/HDStratagemComponent.h"
#include "CharacterTypes/HDCharacterStateTypes.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Controller/HDPlayerController.h"
#include "Weapon/HDWeapon.h"
#include "Stratagem/HDStratagem.h"
#include "GameData/HDStratagemData.h"
#include "Animation/HDAnimInstance.h"
#include "Character/HDCharacterControlData.h"

#define MONTAGESECTIONNAME_RIFLE_AIM FName("Rifle_Aim")
#define MONTAGESECTIONNAME_RIFLE_HIP FName("Rifle_Hip")
#define MONTAGESECTIONNAME_PISTOL FName("Pistol")
#define MONTAGESECTIONNAME_SHOTGUN_AIM FName("Shotgun_Aim")
#define MONTAGESECTIONNAME_SHOTGUN_HIP FName("Shotgun_Hip")
#define MONTAGESECTIONNAME_RIFLE_PRONE FName("Rifle_Prone")
#define MONTAGESECTIONNAME_SHOTGUN_PRONE FName("Shotgun_Prone")
#define SOCKETNAME_RIGHTHAND FName("RightHandSocket")

AHDCharacterPlayer::AHDCharacterPlayer()
    : SpringArm(nullptr)
	, FollowCamera(nullptr)
	, Combat(nullptr)
	, DefaultWeaponClass(nullptr)
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

    Combat = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));
    InputAction = CreateDefaultSubobject<UHDInputActionComponent>(TEXT("InputAction"));
}

void AHDCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SpawnDefaultWeapon();
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
    AHDWeapon* Weapon = GetWeapon();
    if (IsValid(Weapon))
    {
        UHDAnimInstance* HDCharacterInstance = Cast<UHDAnimInstance>(GetMesh()->GetAnimInstance());
        NULL_CHECK(HDCharacterInstance);

        HDCharacterInstance->SetUseUpperSlot(bActive == false);

        Weapon->SetActorTickEnabled(bActive);
        Weapon->SetActorHiddenInGame(bActive == false);
        Weapon->SetActorEnableCollision(bActive);
    }  
}

const float AHDCharacterPlayer::Reload()
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    if (Combat->CanReload() == false)
    {
        return 0.f;
    }

	Combat->Reload();

    const bool bIsShoulder = IsShouldering();
    const EHDCharacterMovementState MovementState = InputAction->GetCharacterMovementState();

	FName SectionName;
	switch (Weapon->GetFireType())
	{
	case EHDFireType::HitScan:
	case EHDFireType::Projectile:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? MONTAGESECTIONNAME_RIFLE_PRONE
			: bIsShoulder ? MONTAGESECTIONNAME_RIFLE_AIM : MONTAGESECTIONNAME_RIFLE_HIP;
		break;
	case EHDFireType::Shotgun:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? MONTAGESECTIONNAME_SHOTGUN_PRONE
			: bIsShoulder ? MONTAGESECTIONNAME_SHOTGUN_AIM : MONTAGESECTIONNAME_SHOTGUN_HIP;
		break;
	}

	CONDITION_CHECK_WITH_RETURNTYPE(SectionName.IsNone(), 0.f);

	PlayMontage(ReloadWeaponMontage, SectionName);

    return Weapon->GetReloadDelay(bIsShoulder);
}

void AHDCharacterPlayer::ReloadFinished()
{
    Combat->ReloadFinished();

    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK(PlayerController);

    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK(Weapon);
    PlayerController->ChangeCapacityHUDInfo(Weapon->GetCapacityCount());
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
    return false;
}

void AHDCharacterPlayer::EquipWeapon(AHDWeapon* NewWeapon)
{
    VALID_CHECK(NewWeapon);

    NewWeapon->SetOwner(this);
    Combat->EquipWeapon(NewWeapon);

    const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(SOCKETNAME_RIGHTHAND);
    NULL_CHECK(RightHandSocket);
    RightHandSocket->AttachActor(NewWeapon, GetMesh());
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
    SetWeaponActive(false);
}

void AHDCharacterPlayer::HoldStratagem()
{   
    if (GetCombatState() != EHDCombatState::HoldStratagem)
    {
        Stratagem->HoldStratagem(GetMesh(), GetHitTarget());
        SetWeaponActive(true);

        SetCombatState(EHDCombatState::HoldStratagem);
    }

    Stratagem->ClearCommand();
}

void AHDCharacterPlayer::CancleStratagem()
{
    VALID_CHECK(Stratagem);

    Stratagem->CancleStratagem();
    SetCombatState(EHDCombatState::Unoccupied);
}

void AHDCharacterPlayer::SpawnDefaultWeapon()
{
    NULL_CHECK(DefaultWeaponClass);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    AHDWeapon* Weapon = World->SpawnActor<AHDWeapon>(DefaultWeaponClass);
    NULL_CHECK(Weapon);

    EquipWeapon(Weapon);
}

void AHDCharacterPlayer::InterpFOV(float DeltaSeconds)
{
    AHDWeapon* Weapon = GetWeapon();
    if (IsValid(Weapon) == false)
    {
        return;
    }

    const bool bIsShoulder = IsShouldering();
    const float TargetFOV = bIsShoulder ? Weapon->GetZoomedFOV() : Combat->GetDefaultFOV();
    const float InterpSpeed = bIsShoulder ? Weapon->GetZoomInterpSpeed() : Combat->GetZoomInterpSpeed();
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
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    if(Combat->Fire(IsPressed) == false)
    {
        if (Combat->CanReload() && Weapon->IsAmmoEmpty())
        {
            Reload();
        }

        return 0.f;
    }

    switch (Weapon->GetFireType())
    {
    case EHDFireType::HitScan:
    case EHDFireType::Projectile:
    {
        const FName SectionName = IsShouldering() ? MONTAGESECTIONNAME_RIFLE_AIM : MONTAGESECTIONNAME_RIFLE_HIP;
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

    return Weapon->GetFireDelay();
}

const bool AHDCharacterPlayer::FireFinished()
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK_WITH_RETURNTYPE(Weapon, false);

    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK_WITH_RETURNTYPE(PlayerController, 0.f);
    PlayerController->ChangeAmmoHUDInfo(Weapon->GetAmmoCount());

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
