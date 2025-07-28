
#include "HDCharacterPlayer.h"
#include "Define/HDMontageSectionNames.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "Component/HDCombatComponent.h"
#include "Component/HDMovementStateComponent.h"
#include "Component/HDStratagemComponent.h"
#include "Component/HDAbilitySystemComponent.h"
#include "Animation/HDAnimInstance.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "Weapon/WeaponTypes.h"
#include "Player/HDGASPlayerState.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameData/HDCharacterControlData.h"

AHDCharacterPlayer::AHDCharacterPlayer()
    : SpringArm(nullptr)
	, FollowCamera(nullptr)
	, Combat(nullptr)
    , MovementState(nullptr)
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

    Combat          = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));
    MovementState   = CreateDefaultSubobject<UHMovementStateComponent>(TEXT("Movement"));
    Stratagem       = CreateDefaultSubobject<UHDStratagemComponent>(TEXT("Stratagem"));
}

void AHDCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    Combat->SpawnDefaultWeapon();
    InitAbilitySystemComponent();
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    if (bRagdoll == false)
    {
        SetCharacterStanceState(EHDCharacterStanceState::Prone, true);

        GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]() {
            SetCombatState(EHDCombatState::Unoccupied);
            }
        ));
    }
}

void AHDCharacterPlayer::SetWeaponActive(const bool bActive)
{
    Combat->SetWeaponActive(bActive);

    UHDAnimInstance* HDCharacterInstance = Cast<UHDAnimInstance>(GetMesh()->GetAnimInstance());
    NULL_CHECK(HDCharacterInstance);
}

const float AHDCharacterPlayer::Reload()
{
    if (Combat->CanReload() == false)
    {
        return 0.f;
    }

	Combat->Reload();

    const bool bIsShoulder = IsShouldering();
    const EHDCharacterStanceState StanceState = MovementState->GetStanceState();

	FName SectionName;
    switch (Combat->GetWeaponFireType())
	{
	case EHDFireType::HitScan:
	case EHDFireType::Projectile:
		SectionName = StanceState == EHDCharacterStanceState::Prone ? HDMONTAGE_SECTIONNAME_RIFLE_PRONE
			: bIsShoulder ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
		break;
	case EHDFireType::Shotgun:
		SectionName = StanceState == EHDCharacterStanceState::Prone ? HDMONTAGE_SECTIONNAME_SHOTGUN_PRONE
			: bIsShoulder ? HDMONTAGE_SECTIONNAME_SHOTGUN_AIM : HDMONTAGE_SECTIONNAME_SHOTGUN_HIP;
		break;
	}

	CONDITION_CHECK_WITH_RETURNTYPE(SectionName.IsNone() == false, 0.f);

    PlayAnimMontage(Combat->GetCombatMontage(EHDCombatMontage::Reload), 1.f, SectionName);

    return Combat->GetWeaponReloadDelay(bIsShoulder);
}

void AHDCharacterPlayer::ReloadFinished()
{
    Combat->ReloadFinished();

    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CAPACITYCHANGE, GetAbilitySystemComponent());
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
    return Combat->GetTurnInPlace();
}

const bool AHDCharacterPlayer::IsUseRotateBone() const
{
    return Combat->IsUseRotateBone();
}

AHDWeapon* AHDCharacterPlayer::GetWeapon() const
{
    return Combat->GetWeapon();
}

const float AHDCharacterPlayer::GetWeaponFireDelay() const
{
    return Combat->GetWeaponFireDelay();
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
    if (CombatState == EHDCombatState::Unoccupied || CombatState == EHDCombatState::Fire)
    {
        Fire(bActive);
    }
    else if (CombatState == EHDCombatState::HoldStratagem)
    {
        // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
        // 실제 AddImpulse는 AnimNotify에서 DetachTiming에 함
        PlayAnimMontage(Combat->GetCombatMontage(EHDCombatMontage::Throw));
        SetCombatState(EHDCombatState::Throwing);
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
    MovementState->SetSpringArmDefaultZOffset(CharacterControlData->TargetOffset.Z);
}

UHDStratagemComponent* AHDCharacterPlayer::GetStratagemComponent()
{
    return Stratagem;
}

const float AHDCharacterPlayer::GetMoveSpeedByState(const EHDCharacterStanceState StanceState, const EHDCharacterMovementState MoveState)
{
    const bool bSprint = MoveState == EHDCharacterMovementState::Sprint;
    FGameplayAttribute Attribute;
    switch (StanceState)
    {
    case EHDCharacterStanceState::Idle:
        Attribute = bSprint ? UHDSpeedAttributeSet::GetSprintSpeedAttribute() : UHDSpeedAttributeSet::GetWalkSpeedAttribute();
        break;
    case EHDCharacterStanceState::Crouch:
        Attribute = UHDSpeedAttributeSet::GetCrouchSpeedAttribute();
        break;
    case EHDCharacterStanceState::Prone:
        Attribute = UHDSpeedAttributeSet::GetCrawlingSpeedAttribute();
        break;
    }

    CONDITION_CHECK_WITH_RETURNTYPE(Attribute.IsValid(), 0.f);
    float Speed = GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
    if (bSprint && StanceState == EHDCharacterStanceState::Crouch)
    {
        Speed *= 1.5f;
    }

    return Speed;
}

void AHDCharacterPlayer::InputStratagemCommand(const FInputActionValue& Value)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC && ASC->HasMatchingGameplayTag(HDTAG_CHARACTER_STATE_STRATAGEMINPUTMODE))
    {
        EHDCommandInput NewCommand = EHDCommandInput::Count;
        const FVector2D NewInput = Value.Get<FVector2D>();
        if (FMath::Abs(NewInput.Y) > 0.5f)
        {
            NewCommand = NewInput.Y > 0.f ? EHDCommandInput::Up : EHDCommandInput::Down;
        }
        else if (FMath::Abs(NewInput.X) > 0.5f)
        {
            NewCommand = NewInput.X > 0.f ? EHDCommandInput::Right : EHDCommandInput::Left;
        }

        if (NewCommand == EHDCommandInput::Count)
        {
            LOG(TEXT("CommandInput is Invalid!!"));
            return;
        }

        GetStratagemComponent()->AddStratagemCommand(NewCommand);

        // HUD 연동용 GAS Event
        FGameplayAbilityHelper::SendGameplayEventToTarget(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND, this, ASC);
    }
}

const EHDCharacterMovementState AHDCharacterPlayer::GetMovementState() const
{
    return MovementState->GetMovementState();
}

void AHDCharacterPlayer::SetMovementState(const EHDCharacterMovementState NewState)
{
    MovementState->SetMovementState(NewState);
    
    const float NewSpeed = GetMoveSpeedByState(MovementState->GetStanceState(), NewState);
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

const EHDCharacterStanceState AHDCharacterPlayer::GetStanceState() const
{
    return MovementState->GetStanceState();
}

void AHDCharacterPlayer::SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
    MovementState->SetStanceState(NewState, bForced);
	if (bForced)
	{
        SetCombatState(NewState == EHDCharacterStanceState::Prone ? EHDCombatState::Ragdoll : EHDCombatState::Unoccupied);
    }

    const float NewSpeed = GetMoveSpeedByState(NewState, MovementState->GetMovementState());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDCharacterPlayer::RestoreStanceState()
{
    MovementState->RestoreStanceState();

    const float NewSpeed = GetMoveSpeedByState(MovementState->GetStanceState(), MovementState->GetMovementState());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
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

    Stratagem->CancelStratagem();
    SetCombatState(EHDCombatState::Unoccupied);
}

void AHDCharacterPlayer::InitAbilitySystemComponent()
{
    AHDGASPlayerState* GASPlayerState = GetPlayerState<AHDGASPlayerState>();
    NULL_CHECK(GASPlayerState);

    SetAbilitySystemComponent(GASPlayerState, GASPlayerState->GetAbilitySystemComponent<UHDAbilitySystemComponent>(), EHDCharacterType::Player);
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

    Combat->SetCombatState(EHDCombatState::Fire);

    switch (Combat->GetWeaponFireType())
    {
    case EHDFireType::HitScan:
    case EHDFireType::Projectile:
    {
        const FName SectionName = IsShouldering() ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
        PlayAnimMontage(Combat->GetCombatMontage(EHDCombatMontage::Fire), 1.f, SectionName);
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
    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_AMMOCHANGE, GetAbilitySystemComponent());

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

    MovementState->SetStanceState(EHDCharacterStanceState::Prone);
}
