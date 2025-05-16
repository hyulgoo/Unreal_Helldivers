// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Player/HDGASPlayerState.h"
#include "EnhancedInputComponent.h"
#include "GameAbility/HDGA_StratagemInputMode.h"
#include "Controller/HDPlayerController.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"
#include "GameData/HDCharacterStat.h"

AHDGASCharacterPlayer::AHDGASCharacterPlayer()
{
}

UAbilitySystemComponent* AHDGASCharacterPlayer::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHDGASCharacterPlayer::InitializeAttributeSet()
{
    NULL_CHECK(AbilitySystemComponent);

    const FHDCharacterStat* InitCharacterStat = GetCharacterStatByArmorType(EHDArmorType::Medium);
    NULL_CHECK(InitCharacterStat);

    UHDHealthAttributeSet* HealthAttributeSet = NewObject<UHDHealthAttributeSet>(this);
    NULL_CHECK(HealthAttributeSet);
    HealthAttributeSet->MaxHealth.SetBaseValue(InitCharacterStat->MaxHealth);

    AbilitySystemComponent->AddAttributeSetSubobject(HealthAttributeSet);

    UHDPlayerSpeedAttributeSet* SpeedAttribute = NewObject<UHDPlayerSpeedAttributeSet>(this);
    NULL_CHECK(SpeedAttribute);
    SpeedAttribute->CrawlingSpeed.SetBaseValue(InitCharacterStat->CrawlingSpeed);
    SpeedAttribute->CrouchSpeed.SetBaseValue(InitCharacterStat->CrouchSpeed);
    SpeedAttribute->WalkSpeed.SetBaseValue(InitCharacterStat->WalkSpeed);
    SpeedAttribute->SprintSpeed.SetBaseValue(InitCharacterStat->SprintSpeed);

    AbilitySystemComponent->AddAttributeSetSubobject(SpeedAttribute);
}

const FHDCharacterStat* AHDGASCharacterPlayer::GetCharacterStatByArmorType(const EHDArmorType NewArmorType) const
{
    NULL_CHECK_WITH_RETURNTYPE(ArmorTypeStatusDataTable, nullptr);

    static const UEnum* EnumPtr = StaticEnum<EHDArmorType>();
    FString ArmorTypetoString = EnumPtr->GetNameStringByValue(static_cast<int64>(NewArmorType));
    FHDCharacterStat* ArmorStatus = ArmorTypeStatusDataTable->FindRow<FHDCharacterStat>(FName(ArmorTypetoString), TEXT("ArmorStatus"));
    return ArmorStatus;
}

void AHDGASCharacterPlayer::SetArmor(EHDArmorType NewArmorType)
{
    NULL_CHECK(AbilitySystemComponent);

    CONDITION_CHECK(ArmorType == NewArmorType);
    ArmorType = NewArmorType;

    const FHDCharacterStat* ArmorStatus = GetCharacterStatByArmorType(NewArmorType);
    NULL_CHECK(ArmorStatus);

    NULL_CHECK(InitStatEffect);
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

void AHDGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    NULL_CHECK(EnhancedInputComponent);

    SetupGASInputComponent(EnhancedInputComponent);
    SetGASEventInputComponent(EnhancedInputComponent);

    InitializeAttributeSet();

    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHDGASCharacterPlayer::SetupGASInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    VALID_CHECK(AbilitySystemComponent);

    FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
    EffectContextHandle.AddSourceObject(this);
    FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitStatEffect, static_cast<int>(ArmorType), EffectContextHandle);

    NULL_CHECK(EnhancedInputComponent);

    for (const FTaggedInputAction& TaggedInputAction : TaggedInputActions)
    {
        if (TaggedInputAction.InputAction && TaggedInputAction.InputTag.IsValid())
        {
            EnhancedInputComponent->BindAction(TaggedInputAction.InputAction, ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputPressed, TaggedInputAction.InputTag);
            EnhancedInputComponent->BindAction(TaggedInputAction.InputAction, ETriggerEvent::Completed, this, &AHDGASCharacterPlayer::GASInputReleased, TaggedInputAction.InputTag);
        }
    }
}

void AHDGASCharacterPlayer::SetGASEventInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    NULL_CHECK(EnhancedInputComponent);
    NULL_CHECK(AbilitySystemComponent);

    for (const FGameplayTag& EventCallTag : EventCallTags.GetGameplayTagArray())
    {
        AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(EventCallTag).AddUObject(this, &AHDGASCharacterPlayer::HandleGameplayEvent);
    }

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

    for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        UHDGameplayAbility* HDAbility = Cast<UHDGameplayAbility>(Spec.Ability);
        if (HDAbility == nullptr)
        {
            continue;
        }

        const FGameplayTagContainer& InputTagContainer = HDAbility->GetAssetInputTags();
        if (InputTagContainer.IsValid() == false || InputTagContainer.HasTagExact(Tag) == false)
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

    for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        UHDGameplayAbility* HDAbility = Cast<UHDGameplayAbility>(Spec.Ability);
        if (HDAbility == nullptr)
        {
            continue;
        }

        const FGameplayTagContainer& InputTagContainer = HDAbility->GetAssetInputTags();
        if (InputTagContainer.IsValid() == false || InputTagContainer.HasTagExact(Tag) == false)
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
            UE_LOG(LogTemp, Warning, TEXT("CommandInput is Invalid!!"));
            return;
        }

        AddStratagemCommand(NewCommand);
    }
}

void AHDGASCharacterPlayer::HandleGameplayEvent(const FGameplayEventData* Payload)
{
    if (Payload->EventTag.MatchesTagExact(HDTAG_EVENT_STRATAGEMHUD_APPEAR))
    {
        SetStratagemHUDAppear(true);
    }
    else if (Payload->EventTag.MatchesTagExact(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR))
    {
        SetStratagemHUDAppear(false);
    }
}

void AHDGASCharacterPlayer::SetStratagemHUDAppear(const bool bAppear)
{
    AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);

    if (PlayerController->IsLocalController())
    {
        PlayerController->SetStratagemHUDAppear(bAppear);
    }
}
