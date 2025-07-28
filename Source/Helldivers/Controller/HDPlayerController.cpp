// Fill out your copyright notice in the Description page of Project Settings.

#include "HDPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Define/HDDefine.h"
#include "Define/HDGameplayTag.h"
#include "Component/HDStratagemComponent.h"
#include "Component/HDCombatComponent.h"
#include "Component/HDInputComponent.h"
#include "Component/HDAbilitySystemComponent.h"
#include "UI/HDGASPlayerUserWidget.h"
#include "UI/HDStratagemHUDUserWidget.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "GameData/HDCharacterControlData.h"
#include "Character/Player/HDCharacterPlayer.h"

AHDPlayerController::AHDPlayerController()
    : AbilitySystemComponent(nullptr)
    , PlayerHUDWidgetClass(nullptr)
    , PlayerHUDWidget(nullptr)
    , StratagemHUDWidgetClass(nullptr)
    , StratagemHUDWidget(nullptr)
    , CurrentCharacterControlType(EHDCharacterControlType::ThirdPerson)
    , InputActionMap{}
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

UHDAbilitySystemComponent* AHDPlayerController::GetAbilitySystemComponent()
{
    if(AbilitySystemComponent == nullptr)
    {
        AbilitySystemComponent = Cast<UHDAbilitySystemComponent>(FGameplayAbilityHelper::GetAbilitySystemComponentFromActor(GetPawn()));
        NULL_CHECK_WITH_RETURNTYPE(AbilitySystemComponent, nullptr);
        SetAbilitySystemComponentBindEventCall(AbilitySystemComponent);
    }

    return AbilitySystemComponent;
}

void AHDPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AHDPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    if(GetPawn<AHDCharacterPlayer>())
    {
        CreateHUDWidget(aPawn);
        SetCharacterControl(EHDCharacterControlType::ThirdPerson);
    }

    ConsoleCommand(TEXT("showdebug abilitysystem"));
}

void AHDPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

    AbilitySystemComponent = nullptr;
}

void AHDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UHDInputComponent* HDInput = Cast<UHDInputComponent>(InputComponent);
    NULL_CHECK(HDInput);

    CONDITION_CHECK(InputActionMap.Num() == static_cast<int32>(EHDCharacterInputAction::Count));

    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::ThirdMove], ETriggerEvent::Triggered, this, &AHDPlayerController::Move);
    HDInput->BindAction(InputActionMap[EHDCharacterInputAction::ThirdLook], ETriggerEvent::Triggered, this, &AHDPlayerController::Look);
    HDInput->SetTaggedInputActionDataAsset(AbilityInputData, this, &AHDPlayerController::AbilityInputTriggered, &AHDPlayerController::AbilityInputReleased, &AHDPlayerController::AbilityInputToggled);
}

void AHDPlayerController::SetAbilitySystemComponentBindEventCall(UHDAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);

    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_PLAYERHUD_AMMOCHANGE).AddUObject(this, &AHDPlayerController::OnPlayerHUDChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_PLAYERHUD_CAPACITYCHANGE).AddUObject(this, &AHDPlayerController::OnPlayerHUDChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND).AddUObject(this, &AHDPlayerController::OnStratagemHUDChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_APPEAR).AddUObject(this, &AHDPlayerController::OnStratagemHUDChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR).AddUObject(this, &AHDPlayerController::OnStratagemHUDChanged);
}

void AHDPlayerController::OnPlayerHUDChanged(const FGameplayEventData* Payload)
{
    NULL_CHECK(Payload);

    if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_AMMOCHANGE)
    {
        VALID_CHECK(PlayerHUDWidget);
        const int32 NewAmmoCount = static_cast<int32>(Payload->EventMagnitude);
        PlayerHUDWidget->OnAmmoCountChanged(NewAmmoCount);
    }
    else if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_CAPACITYCHANGE)
    {
        VALID_CHECK(PlayerHUDWidget);
        const int32 NewCapacityCount = static_cast<int32>(Payload->EventMagnitude);
        PlayerHUDWidget->OnCapacityCountChanged(NewCapacityCount);
    }
}

void AHDPlayerController::OnStratagemHUDChanged(const FGameplayEventData* Payload)
{
    NULL_CHECK(Payload);
    VALID_CHECK(StratagemHUDWidget);

    if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND)
    {
        UHDStratagemComponent* StratagemComponent = Payload->Instigator->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);

        const TArray<FName>& CommandMatchStratagemNameList = StratagemComponent->GetCommandMatchStratagemNameList();
        const int32 CurrentInputNum = StratagemComponent->GetCurrentInputNum();

        StratagemHUDWidget->SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputNum);
    }
    else if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_APPEAR)
    {
        StratagemHUDWidget->WidgetAppear(true);
    }
    else if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR)
    {
        StratagemHUDWidget->WidgetAppear(false);
    }
}

void AHDPlayerController::AbilityInputTriggered(const FGameplayTag InputTag)
{
    UHDAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    const FGameplayTag& AssetTag = AbilityInputData->GetTriggerTagByInputTag(InputTag);
    ASC->AbilityInputTagTriggered(AssetTag);
}

void AHDPlayerController::AbilityInputReleased(const FGameplayTag InputTag)
{
    UHDAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    const FGameplayTag& AssetTag = AbilityInputData->GetTriggerTagByInputTag(InputTag);
    ASC->AbilityInputTagReleased(AssetTag);
}

void AHDPlayerController::AbilityInputToggled(const FGameplayTag InputTag)
{
    UHDAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    const FGameplayTag& AssetTag = AbilityInputData->GetTriggerTagByInputTag(InputTag);
    ASC->AbilityInputTagToggled(AssetTag);
}

void AHDPlayerController::ChangeCharacterControlType()
{
    CurrentCharacterControlType = CurrentCharacterControlType == EHDCharacterControlType::FirstPerson 
        ? EHDCharacterControlType::ThirdPerson : EHDCharacterControlType::FirstPerson;

    SetCharacterControl(CurrentCharacterControlType);
}

void AHDPlayerController::SetCharacterControl(const EHDCharacterControlType NewCharacterControlType)
{
    CONDITION_CHECK(CharacterControlDataMap.Contains(NewCharacterControlType));
    
    CurrentCharacterControlType = NewCharacterControlType;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    NULL_CHECK(Subsystem);

    Subsystem->ClearAllMappings();

    UInputMappingContext* NewMappingContext = CharacterControlDataMap[CurrentCharacterControlType]->InputMappingContext;
    NULL_CHECK(NewMappingContext);

    Subsystem->AddMappingContext(NewMappingContext, 0);

    AHDCharacterPlayer* HDPlayer = GetPawn<AHDCharacterPlayer>();
    NULL_CHECK(HDPlayer);

    HDPlayer->SetCharacterControlData(CharacterControlDataMap[CurrentCharacterControlType]);
}

void AHDPlayerController::CreateHUDWidget(APawn* aPawn)
{
    NULL_CHECK(aPawn);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    if(PlayerHUDWidgetClass)
    {
        if (PlayerHUDWidget)
        {
            PlayerHUDWidget->Destruct();
            PlayerHUDWidget = nullptr;
        }

        PlayerHUDWidget = CreateWidget<UHDGASPlayerUserWidget>(World, PlayerHUDWidgetClass, FName("PlayerHUDWidget"));
        NULL_CHECK(PlayerHUDWidget);

        UHDCombatComponent* Combat = aPawn->GetComponentByClass<UHDCombatComponent>();
        NULL_CHECK(Combat);

        PlayerHUDWidget->SetChangedWeaponAmmoCountInfo(Combat->GetWeaponAmmoCount(), Combat->GetWeaponMaxAmmoCount());
        PlayerHUDWidget->SetChangedWeaponCapacityCountInfo(Combat->GetWeaponCapacityCount(), Combat->GetWeaponMaxCapacityCount());
        PlayerHUDWidget->SetAbilitySystemComponent(GetAbilitySystemComponent());
        PlayerHUDWidget->AddToViewport();
    }
    else
    {
        LOG(TEXT("PlayerHUDWidgetClass is nullptr!"));
    }

    if(StratagemHUDWidgetClass)
    {
        StratagemHUDWidget = CreateWidget<UHDStratagemHUDUserWidget>(World, StratagemHUDWidgetClass, FName("StratagemHUDWidget"));
        NULL_CHECK(StratagemHUDWidget);

        UHDStratagemComponent* StratagemComponent = aPawn->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);

        StratagemHUDWidget->SetStratagemListHUD(StratagemComponent->GetAvaliableStratagemDataTable());
        StratagemHUDWidget->AddToViewport();
    }
    else
    {
        LOG(TEXT("StratagemHUDWidgetClass is nullptr!"));
    }
}

void AHDPlayerController::Look(const FInputActionValue& Value)
{
    const FVector2D& LookAxisVector = Value.Get<FVector2D>();

    AddYawInput(LookAxisVector.X);
    AddPitchInput(LookAxisVector.Y);
}

void AHDPlayerController::Move(const FInputActionValue& Value)
{
    const FVector2D& MovementVector = Value.Get<FVector2D>();
    const FRotator& Rotation = GetControlRotation();

    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector& ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector& RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    APawn* ControlledPawn = GetPawn<APawn>();
    if (ControlledPawn)
    {
        ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.X);
        ControlledPawn->AddMovementInput(RightDirection, MovementVector.Y);
    }
}
