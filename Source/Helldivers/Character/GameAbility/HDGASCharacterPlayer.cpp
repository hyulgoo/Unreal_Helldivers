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
	: AbilitySystemComponent(nullptr)
	, InitStatEffect(nullptr)
    , StartAbilities{}
    , TaggedHoldActions{}
    , TaggedToggleActions{}
    , InputActionMap{}
	, EventCallTags(FGameplayTagContainer())
    , TagEventBindInfoList{}
	, ArmorType(EHDArmorType::Count)
	, ArmorTypeStatusDataTable(nullptr)
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

UAbilitySystemComponent* AHDGASCharacterPlayer::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHDGASCharacterPlayer::SetArmor(const EHDArmorType NewArmorType)
{
    NULL_CHECK(AbilitySystemComponent);
    NULL_CHECK(InitStatEffect);

    CONDITION_CHECK(ArmorType == NewArmorType);
    ArmorType = NewArmorType;

    const FHDCharacterStat* ArmorStatus = GetCharacterStatByArmorType(ArmorType);
    NULL_CHECK(ArmorStatus);

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle InitStatusSpec = AbilitySystemComponent->MakeOutgoingSpec(InitStatEffect, 1.f, Context);
    CONDITION_CHECK(InitStatusSpec.IsValid() == false);

    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXHEALTH,       ArmorStatus->MaxHealth);
    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_CRAWLINGSPEED,   ArmorStatus->CrawlingSpeed);
    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_CROUCHSPEED,     ArmorStatus->CrouchSpeed);
    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_WALKSPEED,       ArmorStatus->WalkSpeed);
    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_SPRINTSPEED,     ArmorStatus->SprintSpeed);
    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXSTAMINA,      ArmorStatus->MaxStamina);

    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitStatusSpec.Data.Get());
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

        EnhancedInputComponent->BindAction(TaggedToggleAction.InputAction, ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputToggled, TaggedToggleAction.InputTag);
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

        Spec.InputPressed = true;
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

        Spec.InputPressed = false;
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
            Spec.InputPressed = false;
            AbilitySystemComponent->AbilitySpecInputReleased(Spec);
        }
        else
        {
            Spec.InputPressed = true;
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

        // HUD 연동용 GAS Event
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

    AbilitySystemComponent->InitStats(UHDHealthAttributeSet::StaticClass(), nullptr);
    AbilitySystemComponent->InitStats(UHDPlayerSpeedAttributeSet::StaticClass(), nullptr);

    // 추후 확장 시 GE로 초기화하게 변경할 것
    const FHDCharacterStat* InitCharacterStat = GetCharacterStatByArmorType(EHDArmorType::Medium);
    NULL_CHECK(InitCharacterStat);

    UHDHealthAttributeSet* HealthAttributeSet = const_cast<UHDHealthAttributeSet*>(AbilitySystemComponent->GetSet<UHDHealthAttributeSet>());
    NULL_CHECK(HealthAttributeSet);
    HealthAttributeSet->MaxHealth.SetBaseValue(InitCharacterStat->MaxHealth);
    HealthAttributeSet->CurrentHealth.SetBaseValue(InitCharacterStat->MaxHealth);

    HealthAttributeSet->CurrentHealth.SetCurrentValue(InitCharacterStat->MaxHealth);
    HealthAttributeSet->MaxHealth.SetCurrentValue(InitCharacterStat->MaxHealth);

    UHDPlayerSpeedAttributeSet* SpeedAttribute = const_cast<UHDPlayerSpeedAttributeSet*>(AbilitySystemComponent->GetSet<UHDPlayerSpeedAttributeSet>());
    NULL_CHECK(SpeedAttribute);
    SpeedAttribute->CrawlingSpeed.SetBaseValue(InitCharacterStat->CrawlingSpeed);
    SpeedAttribute->CrouchSpeed.SetBaseValue(InitCharacterStat->CrouchSpeed);
    SpeedAttribute->WalkSpeed.SetBaseValue(InitCharacterStat->WalkSpeed);
	SpeedAttribute->SprintSpeed.SetBaseValue(InitCharacterStat->SprintSpeed);

	SpeedAttribute->CrawlingSpeed.SetCurrentValue(InitCharacterStat->CrawlingSpeed);
	SpeedAttribute->CrouchSpeed.SetCurrentValue(InitCharacterStat->CrouchSpeed);
	SpeedAttribute->WalkSpeed.SetCurrentValue(InitCharacterStat->WalkSpeed);
	SpeedAttribute->SprintSpeed.SetCurrentValue(InitCharacterStat->SprintSpeed);

    GetCharacterMovement()->MaxWalkSpeed = SpeedAttribute->GetWalkSpeed();
}
const FHDCharacterStat* AHDGASCharacterPlayer::GetCharacterStatByArmorType(const EHDArmorType NewArmorType) const
{
    NULL_CHECK_WITH_RETURNTYPE(ArmorTypeStatusDataTable, nullptr);

    static const UEnum* EnumPtr = StaticEnum<EHDArmorType>();
    FString ArmorTypetoString = EnumPtr->GetNameStringByValue(static_cast<int64>(NewArmorType));
    FHDCharacterStat* ArmorStatus = ArmorTypeStatusDataTable->FindRow<FHDCharacterStat>(FName(ArmorTypetoString), TEXT("ArmorStatus"));
    return ArmorStatus;
}

void AHDGASCharacterPlayer::SetCharacterMovementState(const EHDCharacterMovementState NewState, const bool bForce)
{
    Super::SetCharacterMovementState(NewState, bForce);
    NULL_CHECK(AbilitySystemComponent);
    
    const bool bSprint = IsSprint();
    const float NewSpeed = GetMoveSpeedByMovementStateAndIsSprint(NewState, bSprint);
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDGASCharacterPlayer::RestoreMovementState()
{
	Super::RestoreMovementState();

    const float NewSpeed = GetMoveSpeedByMovementStateAndIsSprint(GetCharacterMovementState(), IsSprint());
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AHDGASCharacterPlayer::SetSprint(const bool bSprint)
{
    Super::SetSprint(bSprint);

    const float NewSpeed = GetMoveSpeedByMovementStateAndIsSprint(GetCharacterMovementState(), bSprint);
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

const float AHDGASCharacterPlayer::GetMoveSpeedByMovementStateAndIsSprint(const EHDCharacterMovementState State, const bool bSprint)
{
    NULL_CHECK_WITH_RETURNTYPE(AbilitySystemComponent, 0.f);

    FGameplayAttribute Attribute;
    switch (State)
    {
    case EHDCharacterMovementState::Idle:
        Attribute = bSprint ? UHDPlayerSpeedAttributeSet::GetSprintSpeedAttribute() : UHDPlayerSpeedAttributeSet::GetWalkSpeedAttribute();
        break;
    case EHDCharacterMovementState::Crouch:
        Attribute = UHDPlayerSpeedAttributeSet::GetCrouchSpeedAttribute();
        break;
    case EHDCharacterMovementState::Prone:
        Attribute = UHDPlayerSpeedAttributeSet::GetCrawlingSpeedAttribute();
        break;
    }

    CONDITION_CHECK_WITH_RETURNTYPE(Attribute.IsValid() == false, 0.f);
    float Speed = AbilitySystemComponent->GetNumericAttribute(Attribute);
    if (bSprint && State == EHDCharacterMovementState::Crouch)
    {
        Speed *= 1.5f;
    }

    return Speed;
}

