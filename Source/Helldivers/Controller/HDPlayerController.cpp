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
#include "UI/HDHUD.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "GameData/HDCharacterControlData.h"
#include "Character/Player/HDCharacterPlayer.h"

AHDPlayerController::AHDPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
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

    if(Cast<AHDCharacterPlayer>(aPawn))
    {
        CreatePlayerWidget(aPawn);
        SetCharacterControl(EHDCharacterControlType::ThirdPerson);

        AHDHUD* HUD = GetHUD<AHDHUD>();
        NULL_CHECK(HUD);

        HUD->CreateDefaultWidget(GetAbilitySystemComponent());

        ConsoleCommand(TEXT("showdebug abilitysystem"));

        FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_EVENT_PLAYERHUD_INITIALIZE, GetAbilitySystemComponent());
    }
}

void AHDPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

    if(AbilitySystemComponent)
    {
        AbilitySystemComponent->GenericGameplayEventCallbacks.Remove(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND);
        AbilitySystemComponent->GenericGameplayEventCallbacks.Remove(HDTAG_EVENT_STRATAGEMHUD_APPEAR);
        AbilitySystemComponent->GenericGameplayEventCallbacks.Remove(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR);

        AbilitySystemComponent = nullptr;
    }
}

void AHDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    NULL_CHECK(EnhancedInput);

    CONDITION_CHECK(InputActionMap.Num() == static_cast<int32>(EHDCharacterInputAction::Count));

    EnhancedInput->BindAction(InputActionMap[EHDCharacterInputAction::ThirdMove], ETriggerEvent::Triggered, this, &ThisClass::Move);
    EnhancedInput->BindAction(InputActionMap[EHDCharacterInputAction::ThirdLook], ETriggerEvent::Triggered, this, &ThisClass::Look);
}

void AHDPlayerController::SetAbilitySystemComponentBindEventCall(UHDAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);

    UHDInputComponent* HDInput = Cast<UHDInputComponent>(InputComponent);
    NULL_CHECK(HDInput);

    HDInput->SetTaggedInputActionDataAsset(AbilityInputData, this, &ThisClass::AbilityInputTriggered, &ThisClass::AbilityInputReleased, &ThisClass::AbilityInputToggled);

    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND).AddUObject(this, &ThisClass::OnStratagemHUDChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_APPEAR).AddUObject(this, &ThisClass::OnStratagemHUDChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR).AddUObject(this, &ThisClass::OnStratagemHUDChanged);
}

void AHDPlayerController::OnStratagemHUDChanged(const FGameplayEventData* Payload)
{
    NULL_CHECK(Payload);
    VALID_CHECK(StratagemWidget);

    if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND)
    {
        UHDStratagemComponent* StratagemComponent = Payload->Instigator->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);

        const TArray<FName>& CommandMatchStratagemNameList = StratagemComponent->GetCommandMatchStratagemNameList();
        const int32 CurrentInputNum = StratagemComponent->GetCurrentInputNum();

        StratagemWidget->SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputNum);
    }
    else if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_APPEAR)
    {
        StratagemWidget->WidgetAppear(true);
    }
    else if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR)
    {
        StratagemWidget->WidgetAppear(false);
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

void AHDPlayerController::CreatePlayerWidget(APawn* aPawn)
{
    NULL_CHECK(aPawn);

    if(StratagemWidgetClass)
    {
        StratagemWidget = CreateWidget<UHDStratagemHUDUserWidget>(GetWorld(), StratagemWidgetClass, FName("StratagemHUDWidget"));
        NULL_CHECK(StratagemWidget);

        UHDStratagemComponent* StratagemComponent = aPawn->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);

        StratagemWidget->SetStratagemListHUD(StratagemComponent->GetAvaliableStratagemDataTable());
        StratagemWidget->AddToViewport();
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
