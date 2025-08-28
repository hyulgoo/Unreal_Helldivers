
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
#include "Player/HDGASPlayerState.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameData/HDCharacterControlData.h"
#include "Weapon\HDWeapon.h"

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

    ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag_Event_PlayerHUD_Initialize).AddUObject(this, &AHDCharacterPlayer::SpawnDefaultWeapon);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag_Character_Action_HoldStratagem).AddUObject(this, &AHDCharacterPlayer::OnStratagemEventReceived);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag_Character_Action_ThrowEnd).AddUObject(this, &AHDCharacterPlayer::OnStratagemEventReceived);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag_Character_Action_DetachStratagem).AddUObject(this, &AHDCharacterPlayer::OnStratagemEventReceived);
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
    }
}

void AHDCharacterPlayer::EquipWeapon(AHDWeapon* NewWeapon)
{
    NULL_CHECK(NewWeapon);
    NULL_CHECK(WeaponAttributeSetEffect);

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    Combat->EquipWeapon(NewWeapon, ASC);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(WeaponAttributeSetEffect, 1.f, ASC->MakeEffectContext());
    CONDITION_CHECK(SpecHandle.IsValid());

    SpecHandle.Data->SetSetByCallerMagnitude(Tag_Data_Attribute_Ammo, Combat->GetWeaponAmmoCount());
    SpecHandle.Data->SetSetByCallerMagnitude(Tag_Data_Attribute_MaxAmmo, Combat->GetWeaponMaxAmmoCount());
    SpecHandle.Data->SetSetByCallerMagnitude(Tag_Data_Attribute_Capacity, Combat->GetWeaponCapacityCount());
    SpecHandle.Data->SetSetByCallerMagnitude(Tag_Data_Attribute_MaxCapacity, Combat->GetWeaponMaxCapacityCount());

    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

UAnimMontage* AHDCharacterPlayer::GetCombatMontage(const EHDCombatMontage MontageType) const
{
    return Combat->GetCombatMontage(MontageType);
}

void AHDCharacterPlayer::PlayWeaponMontage(const EHDCombatMontage MontageType)
{
    USkeletalMeshComponent* WeaponMesh = Combat->GetWeaponMesh();
    NULL_CHECK(WeaponMesh);
    
    FName SectionName = NAME_None;
    if (MontageType == EHDCombatMontage::Fire)
    {
        SectionName = HDMONTAGE_SECTIONNAME_WEAPON_FIRE;
    }
    else if (MontageType == EHDCombatMontage::Reload)
    {
        if(MovementStateComp->GetStanceState() == EHDCharacterStanceState::Prone)
        {
            SectionName = HDMONTAGE_SECTIONNAME_WEAPON_RELOAD_PRONE;
        }
        else
        {
            SectionName = Combat->IsShoulder() ? HDMONTAGE_SECTIONNAME_WEAPON_RELOAD_SHOULDER : HDMONTAGE_SECTIONNAME_WEAPON_RELOAD_HIP;
        }
    }

    CONDITION_CHECK(SectionName != NAME_None);

    UAnimMontage* WeaponMontage = Combat->GetWeaponMontage();
    NULL_CHECK(WeaponMontage);

    float PlayRate = 1.f;

    if(MontageType == EHDCombatMontage::Fire)
    {
        const int32 SectionIndex = WeaponMontage->GetSectionIndex(SectionName);
        const float MontageLength = WeaponMontage->GetSectionLength(SectionIndex); 
        PlayRate = MontageLength / Combat->GetWeaponFireDelay();
    }

    UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
    NULL_CHECK(WeaponAnimInstance);

    WeaponAnimInstance->Montage_Play(WeaponMontage, PlayRate);
    WeaponAnimInstance->Montage_JumpToSection(SectionName, WeaponMontage);
}

bool AHDCharacterPlayer::IsEquippedWeapon() const
{
    return Combat->IsEquippedWeapon();
}

bool AHDCharacterPlayer::IsShoulder() const
{
    return Combat->IsShoulder();
}

bool AHDCharacterPlayer::IsWeaponAutoFire() const
{
    return Combat->IsWeaponAutoFire();
}

void AHDCharacterPlayer::SpawnProjectile()
{
    Combat->SpawnProjectile();
}

void AHDCharacterPlayer::SetWeaponActive(const bool bActive)
{
    Combat->SetWeaponActive(bActive);
}

void AHDCharacterPlayer::SpawnDefaultWeapon(const FGameplayEventData* Payload)
{
    AHDWeapon* Weapon = Combat->SpawnDefaultWeapon();
    NULL_CHECK(Weapon);
    EquipWeapon(Weapon);

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);
    ASC->GenericGameplayEventCallbacks.Remove(Tag_Event_PlayerHUD_Initialize);
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    Combat->SetShoulder(bShoulder);   
}

float AHDCharacterPlayer::GetWeaponFireDelay() const
{
    return Combat->GetWeaponFireDelay();
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

void AHDCharacterPlayer::UpdateSpeed(const EHDCharacterStanceState StanceState, const EHDCharacterMovementState MoveState)
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

    CONDITION_CHECK(Attribute.IsValid());
    float Speed = GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
    if (bSprint && StanceState == EHDCharacterStanceState::Crouch)
    {
        Speed *= 1.5f;
    }

    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AHDCharacterPlayer::InputStratagemCommand(const FInputActionValue& Value)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC && ASC->HasMatchingGameplayTag(Tag_Character_Action_StratagemInputMode))
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

        // HUD ¿¬µ¿¿ë GAS Event
        FGameplayAbilityHelper::SendGameplayEventToTarget(Tag_Event_StratagemHUD_AddCommand, this, ASC);
    }
}

void AHDCharacterPlayer::SetMovementState(const EHDCharacterMovementState NewState)
{
    MovementStateComp->SetMovementState(NewState);
    
    UpdateSpeed(MovementStateComp->GetStanceState(), NewState);
}

void AHDCharacterPlayer::SetCharacterStanceState(const EHDCharacterStanceState NewState, const bool bForced)
{
    MovementStateComp->SetStanceState(NewState, bForced);

    UpdateSpeed(NewState, MovementStateComp->GetMovementState());
}

void AHDCharacterPlayer::RestoreStanceState()
{
    MovementStateComp->RestoreStanceState();

    UpdateSpeed(MovementStateComp->GetStanceState(), MovementStateComp->GetMovementState());
}

void AHDCharacterPlayer::OnStratagemEventReceived(const FGameplayEventData* Payload)
{
    if (Payload->EventTag == Tag_Character_Action_HoldStratagem)
    {
        TryHoldStratagem();
    }
    else if (Payload->EventTag == Tag_Character_Action_DetachStratagem)
    {
        Stratagem->ThrowFinished();
    }
    else if(Payload->EventTag == Tag_Character_Action_ThrowEnd)
    {
        SetWeaponActive(true);
    }
}

void AHDCharacterPlayer::TryHoldStratagem()
{   
    const bool CanHoldStrategem = Stratagem->IsSelectedStratagemExist();
    if (CanHoldStrategem)
    {
        Stratagem->HoldStratagem(GetMesh(), Combat->GetHitTarget());
        SetWeaponActive(false);
        UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
        NULL_CHECK(ASC);
        ASC->AddLooseGameplayTag(Tag_Character_Action_HoldStratagem);
    }
    else
    {
        Stratagem->CancelStratagem();
    }

    Stratagem->ClearCommand();
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
