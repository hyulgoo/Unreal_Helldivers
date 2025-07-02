// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGASCharacterPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "Character/HDCharacterControlData.h"
#include "Player/HDGASPlayerState.h"
#include "Controller/HDPlayerController.h"
#include "Component/HDStratagemComponent.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"
#include "GameData/HDCharacterStat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GameplayAbilityHelper.h"

AHDGASCharacterPlayer::AHDGASCharacterPlayer()
	: InitStatEffect(nullptr)
    , StartAbilities{}
    , TaggedHoldActions{}
    , TaggedToggleActions{}
    , InputActionMap{}
	, EventCallTags(FGameplayTagContainer())
    , TagEventBindInfoList{}
    , CurrentCharacterControlType(EHDCharacterControlType::ThirdPerson)
    , CharacterControlDataMap{}
{
    static ConstructorHelpers::FObjectFinder<UHDCharacterControlData> ThirdPersonDataRef(TEXT("/Script/Helldivers.HDCharacterControlData'/Game/Helldivers/CharacterControl/HDC_ThirdPerson.HDC_ThirdPerson'"));
    if (ThirdPersonDataRef.Succeeded())
    {
        CharacterControlDataMap.Add(EHDCharacterControlType::ThirdPerson, ThirdPersonDataRef.Object);
    }

    static ConstructorHelpers::FObjectFinder<UHDCharacterControlData> FirstPersonDataRef(TEXT("/Script/Helldivers.HDCharacterControlData'/Game/Helldivers/CharacterControl/HDC_FirstPerson.HDC_FirstPerson'"));
    if (FirstPersonDataRef.Succeeded())
    {
        CharacterControlDataMap.Add(EHDCharacterControlType::FirstPerson, FirstPersonDataRef.Object);
    }
}

void AHDGASCharacterPlayer::ChangeCharacterControlType()
{
	SetCharacterControl(CurrentCharacterControlType == EHDCharacterControlType::FirstPerson ?
		EHDCharacterControlType::ThirdPerson : EHDCharacterControlType::FirstPerson);
}

void AHDGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    NULL_CHECK(EnhancedInputComponent);

    SetupGASInputComponent(EnhancedInputComponent);
    SetGASEventInputComponent(EnhancedInputComponent);
}

void AHDGASCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    InitAbilitySystemComponent();
    SetCharacterControl(CurrentCharacterControlType);
}

void AHDGASCharacterPlayer::SetupGASInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    NULL_CHECK(EnhancedInputComponent);

    for (const FTaggedInputAction& TaggedHoldAction : TaggedHoldActions)
    {
        CONDITION_CHECK((TaggedHoldAction.InputAction == nullptr || TaggedHoldAction.InputTag.IsValid()) == false);

		EnhancedInputComponent->BindAction(TaggedHoldAction.InputAction, ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputPressed, TaggedHoldAction.InputTag);
		EnhancedInputComponent->BindAction(TaggedHoldAction.InputAction, ETriggerEvent::Completed, this, &AHDGASCharacterPlayer::GASInputReleased, TaggedHoldAction.InputTag);
    }

    for (const FTaggedInputAction& TaggedToggleAction : TaggedToggleActions)
    {
        if (TaggedToggleAction.InputAction == nullptr || TaggedToggleAction.InputTag.IsValid() == false)
        {
            LOG(TEXT("TaggedToggleAction is InValid!"));
            continue;
        }

        EnhancedInputComponent->BindAction(TaggedToggleAction.InputAction, ETriggerEvent::Started, this, &AHDGASCharacterPlayer::GASInputToggled, TaggedToggleAction.InputTag);
    }
    
    CONDITION_CHECK(InputActionMap.Num() != static_cast<uint8>(EHDCharacterInputAction::Count));

    EnhancedInputComponent->BindAction(InputActionMap[EHDCharacterInputAction::ThirdLook],        ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::ThirdPersonLook);
    EnhancedInputComponent->BindAction(InputActionMap[EHDCharacterInputAction::ThirdMove],        ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::ThirdPersonMove);
    EnhancedInputComponent->BindAction(InputActionMap[EHDCharacterInputAction::FirstLook],        ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::FirstPersonLook);
    EnhancedInputComponent->BindAction(InputActionMap[EHDCharacterInputAction::FirstMove],        ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::FirstPersonMove);
    EnhancedInputComponent->BindAction(InputActionMap[EHDCharacterInputAction::ChangeControl],    ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::ChangeCharacterControlType);
}

void AHDGASCharacterPlayer::SetGASEventInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    NULL_CHECK(EnhancedInputComponent);

    for (const FTagEventBindInfo& TagEventBindInfo : TagEventBindInfoList)
    {
       if(TagEventBindInfo.BindFunctionName.IsValid() && TagEventBindInfo.InputAction)
       {
           EnhancedInputComponent->BindAction(TagEventBindInfo.InputAction, ETriggerEvent::Triggered, this, TagEventBindInfo.BindFunctionName);
       }
    }
}

void AHDGASCharacterPlayer::GASInputPressed(const FGameplayTag Tag)
{
    VALID_CHECK(AbilitySystemComponent);

    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = AbilitySystemComponent->GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySystemComponent->AbilitySpecInputPressed(Spec);
        }
        else
        {
            AbilitySystemComponent->TryActivateAbility(Spec.Handle);
        }
    }
}

void AHDGASCharacterPlayer::GASInputReleased(const FGameplayTag Tag)
{
    VALID_CHECK(AbilitySystemComponent);

    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = AbilitySystemComponent->GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySystemComponent->AbilitySpecInputReleased(Spec);
        }
    }
}

void AHDGASCharacterPlayer::GASInputToggled(const FGameplayTag Tag)
{
    VALID_CHECK(AbilitySystemComponent);

    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = AbilitySystemComponent->GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySystemComponent->AbilitySpecInputReleased(Spec);
        }
        else
        {
            AbilitySystemComponent->TryActivateAbility(Spec.Handle);
        }
    }
}

void AHDGASCharacterPlayer::InputStratagemCommand(const FInputActionValue& Value)
{
    if (AbilitySystemComponent->HasMatchingGameplayTag(HDTAG_CHARACTER_STATE_STRATAGEMINPUTMODE))
    {
        EHDCommandInput NewCommand = EHDCommandInput::Count;
        const FVector2D Input = Value.Get<FVector2D>();
        if(FMath::Abs(Input.Y) > 0.5f)
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

        // HUD ¿¬µ¿¿ë GAS Event
        FGameplayAbilityHelper::SendGameplayEventToTarget(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND, this, AbilitySystemComponent);
    }
}

void AHDGASCharacterPlayer::InitAbilitySystemComponent()
{
    AHDGASPlayerState* GASPlayerState = GetPlayerState<AHDGASPlayerState>();
    NULL_CHECK(GASPlayerState);

    AbilitySystemComponent = GASPlayerState->GetAbilitySystemComponent();
    NULL_CHECK(AbilitySystemComponent);

    AbilitySystemComponent->InitAbilityActorInfo(GASPlayerState, this);

    for (const TSubclassOf<UGameplayAbility>& StartAbility : StartAbilities)
    {
        AbilitySystemComponent->GiveAbility(StartAbility);
    }

    InitializeAttributeSet();
}

void AHDGASCharacterPlayer::InitializeAttributeSet()
{
    NULL_CHECK(AbilitySystemComponent);

    UHDHealthAttributeSet* HealthAttributeSet = NewObject<UHDHealthAttributeSet>(this);
    UHDPlayerSpeedAttributeSet* SpeedAttributeSet = NewObject<UHDPlayerSpeedAttributeSet>(this);

    AbilitySystemComponent->AddAttributeSetSubobject(HealthAttributeSet);
    AbilitySystemComponent->AddAttributeSetSubobject(SpeedAttributeSet);

    SetAttributeStatByArmor(EHDArmorType::Medium);
}
const FHDCharacterStat* AHDGASCharacterPlayer::GetCharacterStatByArmorType(const EHDArmorType NewArmorType) const
{
    NULL_CHECK_WITH_RETURNTYPE(ArmorTypeStatusDataTable, nullptr);

    static const UEnum* EnumPtr = StaticEnum<EHDArmorType>();
    FString ArmorTypetoString = EnumPtr->GetNameStringByValue(static_cast<int64>(NewArmorType));
    FHDCharacterStat* ArmorStatus = ArmorTypeStatusDataTable->FindRow<FHDCharacterStat>(FName(ArmorTypetoString), TEXT("ArmorStatus"));
    return ArmorStatus;
}

void AHDGASCharacterPlayer::RestorePoseState()
{
	Super::RestorePoseState();

    const float NewSpeed = GetMoveSpeedByMovementStateAndIsSprint(GetCharacterPoseState(), IsSprint());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDGASCharacterPlayer::SetSprint(const bool bSprint)
{
    Super::SetSprint(bSprint);

    const float NewSpeed = GetMoveSpeedByMovementStateAndIsSprint(GetCharacterPoseState(), bSprint);
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDGASCharacterPlayer::ThirdPersonLook(const FInputActionValue& Value)
{
    const FVector2D& LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void AHDGASCharacterPlayer::ThirdPersonMove(const FInputActionValue& Value)
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

void AHDGASCharacterPlayer::FirstPersonLook(const FInputActionValue& Value)
{
    const FVector2D& LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(-LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void AHDGASCharacterPlayer::FirstPersonMove(const FInputActionValue& Value)
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

void AHDGASCharacterPlayer::SetCharacterControl(const EHDCharacterControlType NewCharacterControlType)
{
    UHDCharacterControlData* NewCharacterControl = CharacterControlDataMap[NewCharacterControlType];
    NULL_CHECK(NewCharacterControl);

    SetCharacterControlData(NewCharacterControl);

    APlayerController* PlayerController = GetController<APlayerController>();
    NULL_CHECK(PlayerController);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    NULL_CHECK(Subsystem);

    Subsystem->ClearAllMappings();
    UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
    if (NewMappingContext)
    {
        Subsystem->AddMappingContext(NewMappingContext, 0);
    }

    CurrentCharacterControlType = NewCharacterControlType;
}

const float AHDGASCharacterPlayer::GetMoveSpeedByMovementStateAndIsSprint(const EHDCharacterPoseState State, const bool bSprint)
{
    NULL_CHECK_WITH_RETURNTYPE(AbilitySystemComponent, 0.f);

    FGameplayAttribute Attribute;
    switch (State)
    {
    case EHDCharacterPoseState::Idle:
        Attribute = bSprint ? UHDPlayerSpeedAttributeSet::GetSprintSpeedAttribute() : UHDPlayerSpeedAttributeSet::GetWalkSpeedAttribute();
        break;
    case EHDCharacterPoseState::Crouch:
        Attribute = UHDPlayerSpeedAttributeSet::GetCrouchSpeedAttribute();
        break;
    case EHDCharacterPoseState::Prone:
        Attribute = UHDPlayerSpeedAttributeSet::GetCrawlingSpeedAttribute();
        break;
    }

    CONDITION_CHECK_WITH_RETURNTYPE(Attribute.IsValid() == false, 0.f);
    float Speed = AbilitySystemComponent->GetNumericAttribute(Attribute);
    if (bSprint && State == EHDCharacterPoseState::Crouch)
    {
        Speed *= 1.5f;
    }

    return Speed;
}

