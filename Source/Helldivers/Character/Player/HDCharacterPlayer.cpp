
#include "HDCharacterPlayer.h"
#include "Define/HDMontageSectionNames.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Component/HDCombatComponent.h"
#include "Component/HDMovementStateComponent.h"
#include "Component/HDStratagemComponent.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "Weapon/WeaponTypes.h"
#include "Player/HDGASPlayerState.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameData/HDCharacterControlData.h"

AHDCharacterPlayer::AHDCharacterPlayer(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
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

    Combat              = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));
    MovementStateComp   = CreateDefaultSubobject<UHDMovementStateComponent>(TEXT("MovementState"));
    Stratagem           = CreateDefaultSubobject<UHDStratagemComponent>(TEXT("Stratagem"));
}

void AHDCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    InitAbilitySystemComponent();

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_PLAYERHUD_INITIALIZE).AddUObject(this, &AHDCharacterPlayer::SpawnDefaultWeapon);
}

void AHDCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    NULL_CHECK(EnhancedInputComponent);

    UInputAction* CommandInput = Stratagem->GetCommandInputAction();
    NULL_CHECK(CommandInput);

    EnhancedInputComponent->BindAction(CommandInput, ETriggerEvent::Triggered, this, &AHDCharacterPlayer::InputStratagemCommand);
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    if (bRagdoll == false)
    {
        SetCharacterStanceState(EHDCharacterStanceState::Prone, true);

        GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]() {
            Combat->SetCombatState(EHDCombatState::Unoccupied);
            }
        ));
    }
}

void AHDCharacterPlayer::EquipWeapon(AHDWeapon* NewWeapon)
{
    NULL_CHECK(NewWeapon);

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    Combat->EquipWeapon(NewWeapon, ASC);

    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_EQUIPWEAPON,          ASC, Combat->GetWeaponMaxCapacityCount());
    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_MAXCAPACITYCHANGE,    ASC, Combat->GetWeaponMaxCapacityCount());
    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_MAXAMMOCHANGE,        ASC, Combat->GetWeaponMaxAmmoCount());
    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CURRENTCAPACITYCHANGE, ASC, Combat->GetWeaponCapacityCount());
    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CURRENTAMMOCHANGE,    ASC, Combat->GetWeaponAmmoCount());
}

AHDWeapon* AHDCharacterPlayer::GetWeapon() const
{
    return Combat->GetWeapon();
}

void AHDCharacterPlayer::SetWeaponActive(const bool bActive)
{
    Combat->SetWeaponActive(bActive);
}

const float AHDCharacterPlayer::Reload()
{
    CONDITION_CHECK_WITH_RETURNTYPE(Combat->CanReload(), 0.f);

	Combat->Reload();

    const bool bIsShoulder = Combat->IsShoulder();
    const EHDCharacterStanceState StanceState = MovementStateComp->GetStanceState();

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

    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CURRENTCAPACITYCHANGE, GetAbilitySystemComponent(), Combat->GetWeaponCapacityCount());
    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CURRENTAMMOCHANGE, GetAbilitySystemComponent(), Combat->GetWeaponAmmoCount());
}

void AHDCharacterPlayer::SpawnDefaultWeapon(const FGameplayEventData* Payload)
{
    AHDWeapon* Weapon = Combat->SpawnDefaultWeapon();
    NULL_CHECK(Weapon);
    EquipWeapon(Weapon);

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);
    ASC->GenericGameplayEventCallbacks.Remove(HDTAG_EVENT_PLAYERHUD_INITIALIZE);
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    Combat->SetShoulder(bShoulder);   
}

const float AHDCharacterPlayer::GetWeaponFireDelay() const
{
    return Combat->GetWeaponFireDelay();
}

void AHDCharacterPlayer::Attack(const bool bAttack)
{
    if(bAttack == false)
    {
        Combat->SetCombatState(EHDCombatState::Unoccupied);
        return;
    }

    const EHDCombatState CombatState = Combat->GetCombatState();
    if (CombatState == EHDCombatState::Unoccupied || CombatState == EHDCombatState::Fire)
    {
        Fire();
    }
    else if (CombatState == EHDCombatState::HoldStratagem)
    {
        // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
        // 실제 AddImpulse는 AnimNotify에서 DetachTiming에 함
        PlayAnimMontage(Combat->GetCombatMontage(EHDCombatMontage::Throw));
        Combat->SetCombatState(EHDCombatState::Throwing);
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
    MovementStateComp->SetSpringArmDefaultZOffset(CharacterControlData->TargetOffset.Z);
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

        CONDITION_CHECK(NewCommand != EHDCommandInput::Count);

        Stratagem->AddStratagemCommand(NewCommand);

        // HUD 연동용 GAS Event
        FGameplayAbilityHelper::SendGameplayEventToTarget(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND, this, ASC);
    }
}

void AHDCharacterPlayer::SetMovementState(const EHDCharacterMovementState NewState)
{
    MovementStateComp->SetMovementState(NewState);
    
    const float NewSpeed = GetMoveSpeedByState(MovementStateComp->GetStanceState(), NewState);
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDCharacterPlayer::SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
    MovementStateComp->SetStanceState(NewState, bForced);
	if (bForced)
	{
        Combat->SetCombatState(NewState == EHDCharacterStanceState::Prone ? EHDCombatState::Ragdoll : EHDCombatState::Unoccupied);
    }

    const float NewSpeed = GetMoveSpeedByState(NewState, MovementStateComp->GetMovementState());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDCharacterPlayer::RestoreStanceState()
{
    MovementStateComp->RestoreStanceState();

    const float NewSpeed = GetMoveSpeedByState(MovementStateComp->GetStanceState(), MovementStateComp->GetMovementState());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDCharacterPlayer::DetachStratagemWhileThrow()
{
    Stratagem->ThrowFinished();
    Combat->SetCombatState(EHDCombatState::Unoccupied);
    SetWeaponActive(true);
}

void AHDCharacterPlayer::TryHoldStratagem()
{   
    const bool CanHoldStrategem = Stratagem->IsSelectedStratagemExist() && Combat->GetCombatState() != EHDCombatState::HoldStratagem;
    if (CanHoldStrategem)
    {
        Stratagem->HoldStratagem(GetMesh(), Combat->GetHitTarget());
        SetWeaponActive(false);

        Combat->SetCombatState(EHDCombatState::HoldStratagem);
    }
    else
    {
        CancleStratagem();
    }

    Stratagem->ClearCommand();
}

void AHDCharacterPlayer::CancleStratagem()
{
    CONDITION_CHECK_WITHOUT_LOG(Combat->GetCombatState() != EHDCombatState::HoldStratagem);

    Stratagem->CancelStratagem();
    Combat->SetCombatState(EHDCombatState::Unoccupied);
}

void AHDCharacterPlayer::InitAbilitySystemComponent()
{
    AHDGASPlayerState* GASPlayerState = GetPlayerState<AHDGASPlayerState>();
    NULL_CHECK(GASPlayerState);

    SetAbilitySystemComponent(GASPlayerState, GASPlayerState->GetAbilitySystemComponent());
}

void AHDCharacterPlayer::InterpFOV(float DeltaSeconds)
{
    const bool bIsShoulder = Combat->IsShoulder();
    const float TargetFOV = bIsShoulder ? Combat->GetWeaponZoomedFOV() : Combat->GetDefaultFOV();
    const float InterpSpeed = bIsShoulder ? Combat->GetWeaponZoomInterpSpeed() : Combat->GetZoomInterpSpeed();
    const float NewInterpFOV = FMath::FInterpTo(Combat->GetCurrentFOV(), TargetFOV, DeltaSeconds, InterpSpeed);
    Combat->SetCurrentFOV(NewInterpFOV);
    
    FollowCamera->SetFieldOfView(Combat->GetCurrentFOV());
}

void AHDCharacterPlayer::Fire()
{
    if(Combat->Fire() == false)
    {
        if (Combat->NeedReload())
        {
            Reload();
        }
        else
        {
            Combat->SetCombatState(EHDCombatState::Unoccupied);
        }

        return;
    }

    switch (Combat->GetWeaponFireType())
    {
    case EHDFireType::HitScan:
    case EHDFireType::Projectile:
    {
        const FName SectionName = Combat->IsShoulder() ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
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

    FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CURRENTAMMOCHANGE, GetAbilitySystemComponent(), Combat->GetWeaponAmmoCount());
}

const bool AHDCharacterPlayer::ContinueFire()
{
    if(Combat->ContinueFire())
    {
        FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_CURRENTAMMOCHANGE, GetAbilitySystemComponent(), Combat->GetWeaponAmmoCount());
        return true;
    }

    return false;
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

    MovementStateComp->SetStanceState(EHDCharacterStanceState::Prone);
}
