
#include "HDCharacterPlayer.h"
#include "Define/HDDefine.h"
#include "Define/HDMontageSectionNames.h"
#include "Define/HDGameplayTag.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/HDCombatComponent.h"
#include "Component/HDInputActionComponent.h"
#include "Component/HDStratagemComponent.h"
#include "Component/HDAbilitySystemComponent.h"
#include "Controller/HDPlayerController.h"
#include "Animation/HDAnimInstance.h"
#include "GameData/HDCharacterControlData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapon/WeaponTypes.h"
#include "Player/HDGASPlayerState.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

AHDCharacterPlayer::AHDCharacterPlayer()
    : SpringArm(nullptr)
	, FollowCamera(nullptr)
	, Combat(nullptr)
    , InputAction(nullptr)
    , Stratagem(nullptr)
    , TaggedInputDataAsset(nullptr)
    , EventCallTags(FGameplayTagContainer())
    , TagEventBindInfoList{}
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

void AHDCharacterPlayer::ChangeCharacterControlType()
{
    EHDCharacterControlType NewControlType = InputAction->GetControlType() == EHDCharacterControlType::FirstPerson 
        ? EHDCharacterControlType::ThirdPerson : EHDCharacterControlType::FirstPerson;
    SetCharacterControl(NewControlType);
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
    SetCharacterControl(InputAction->GetControlType());
}

void AHDCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    NULL_CHECK(EnhancedInputComponent);

    SetupAbilitySystemInputComponent(EnhancedInputComponent);
    SetupEventAbilitySystemInputComponent(EnhancedInputComponent);
}

void AHDCharacterPlayer::SetupAbilitySystemInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    NULL_CHECK(EnhancedInputComponent);

    UHDInputActionComponent* HDInput = Cast<UHDInputActionComponent>(EnhancedInputComponent);
    NULL_CHECK(HDInput);

    TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>> InputActionMap = InputAction->GetInputActionMap();

    CONDITION_CHECK(InputActionMap.Num() != static_cast<uint8>(EHDCharacterInputAction::Count));

    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::ThirdLook], ETriggerEvent::Triggered, this, &AHDCharacterPlayer::ThirdPersonLook);
    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::ThirdMove], ETriggerEvent::Triggered, this, &AHDCharacterPlayer::ThirdPersonMove);
    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::FirstLook], ETriggerEvent::Triggered, this, &AHDCharacterPlayer::FirstPersonLook);
    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::FirstMove], ETriggerEvent::Triggered, this, &AHDCharacterPlayer::FirstPersonMove);
    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::ChangeControl], ETriggerEvent::Triggered, this, &AHDCharacterPlayer::ChangeCharacterControlType);
    HDInput->SetTaggedInputActionDataAsset(TaggedInputDataAsset, this, &AHDCharacterPlayer::AbilityInputTriggered, &AHDCharacterPlayer::AbilityInputReleased, &AHDCharacterPlayer::AbilityInputToggled);
}

void AHDCharacterPlayer::SetupEventAbilitySystemInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    NULL_CHECK(EnhancedInputComponent);

    for (const FTagEventBindInfo& TagEventBindInfo : TagEventBindInfoList)
    {
        if (TagEventBindInfo.BindFunctionName.IsValid() && TagEventBindInfo.InputAction)
        {
            EnhancedInputComponent->BindAction(TagEventBindInfo.InputAction, ETriggerEvent::Triggered, this, TagEventBindInfo.BindFunctionName);
        }
    }
}

void AHDCharacterPlayer::AbilityInputTriggered(const FGameplayTag Tag)
{
    UHDAbilitySystemComponent* HDASC = Cast<UHDAbilitySystemComponent>(GetAbilitySystemComponent());
    NULL_CHECK(HDASC);

    HDASC->AbilityInputTagTriggered(Tag);
}

void AHDCharacterPlayer::AbilityInputReleased(const FGameplayTag Tag)
{
    UHDAbilitySystemComponent* HDASC = Cast<UHDAbilitySystemComponent>(GetAbilitySystemComponent());
    NULL_CHECK(HDASC);

    HDASC->AbilityInputTagReleased(Tag);
}

void AHDCharacterPlayer::AbilityInputToggled(const FGameplayTag Tag)
{
    UHDAbilitySystemComponent* HDASC = Cast<UHDAbilitySystemComponent>(GetAbilitySystemComponent());
    NULL_CHECK(HDASC);

    HDASC->AbilityInputTagToggled(Tag);
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    if (bRagdoll == false)
    {
        SetCharacterStanceState(EHDCharacterStanceState::Prone, true);

        GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this](void) {
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
    const EHDCharacterStanceState MovementState = InputAction->GetStanceState();

	FName SectionName;
    switch (Combat->GetWeaponFireType())
	{
	case EHDFireType::HitScan:
	case EHDFireType::Projectile:
		SectionName = MovementState == EHDCharacterStanceState::Prone ? HDMONTAGE_SECTIONNAME_RIFLE_PRONE
			: bIsShoulder ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
		break;
	case EHDFireType::Shotgun:
		SectionName = MovementState == EHDCharacterStanceState::Prone ? HDMONTAGE_SECTIONNAME_SHOTGUN_PRONE
			: bIsShoulder ? HDMONTAGE_SECTIONNAME_SHOTGUN_AIM : HDMONTAGE_SECTIONNAME_SHOTGUN_HIP;
		break;
	}

	CONDITION_CHECK_WITH_RETURNTYPE(SectionName.IsNone(), 0.f);

    PlayAnimMontage(Combat->GetCombatMontage(EHDCombatMontage::Reload), 1.f, SectionName);

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

    CONDITION_CHECK_WITH_RETURNTYPE(Attribute.IsValid() == false, 0.f);
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
        const FVector2D Input = Value.Get<FVector2D>();
        if (FMath::Abs(Input.Y) > 0.5f)
        {
            NewCommand = Input.Y > 0.f ? EHDCommandInput::Up : EHDCommandInput::Down;
        }
        else if (FMath::Abs(Input.X) > 0.5f)
        {
            NewCommand = Input.X > 0.f ? EHDCommandInput::Right : EHDCommandInput::Left;
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
    return InputAction->GetMovementState();
}

void AHDCharacterPlayer::SetMovementState(const EHDCharacterMovementState NewState)
{
    InputAction->SetMovementState(NewState);
    
    const float NewSpeed = GetMoveSpeedByState(InputAction->GetStanceState(), NewState);
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

const EHDCharacterStanceState AHDCharacterPlayer::GetStanceState() const
{
    return InputAction->GetStanceState();
}

void AHDCharacterPlayer::SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
    InputAction->SetStanceState(NewState, bForced);
	if (bForced)
	{
        SetCombatState(NewState == EHDCharacterStanceState::Prone ? EHDCombatState::Ragdoll : EHDCombatState::Unoccupied);
    }

    const float NewSpeed = GetMoveSpeedByState(NewState, InputAction->GetMovementState());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDCharacterPlayer::RestoreStanceState()
{
    InputAction->RestoreStanceState();

    const float NewSpeed = GetMoveSpeedByState(InputAction->GetStanceState(), InputAction->GetMovementState());
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

    SetAbilitySystemComponent(GASPlayerState, GASPlayerState->GetAbilitySystemComponent(), EHDCharacterType::Player);
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

void AHDCharacterPlayer::ThirdPersonLook(const FInputActionValue& Value)
{
    const FVector2D& LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void AHDCharacterPlayer::ThirdPersonMove(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling())
    {
        return;
    }

    const FVector2D& MovementVector = Value.Get<FVector2D>();
    const FRotator& Rotation = Controller->GetControlRotation();

    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector& ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector& RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void AHDCharacterPlayer::FirstPersonLook(const FInputActionValue& Value)
{
    const FVector2D& LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(-LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void AHDCharacterPlayer::FirstPersonMove(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling())
    {
        return;
    }

    const FVector2D& MovementVector = Value.Get<FVector2D>();

    const FRotator& Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector& ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector& RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void AHDCharacterPlayer::SetCharacterControl(const EHDCharacterControlType NewCharacterControlType)
{
    UHDCharacterControlData* ControlData = InputAction->SetControlType(NewCharacterControlType);

    APlayerController* PlayerController = GetController<APlayerController>();
    NULL_CHECK(PlayerController);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    NULL_CHECK(Subsystem);

    Subsystem->ClearAllMappings();
    UInputMappingContext* NewMappingContext = ControlData->InputMappingContext;
    NULL_CHECK(NewMappingContext);

    Subsystem->AddMappingContext(NewMappingContext, 0);
    SetCharacterControlData(ControlData);
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

    InputAction->SetStanceState(EHDCharacterStanceState::Prone);
}
