// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Player/HDGASPlayerState.h"
#include "EnhancedInputComponent.h"
#include "GameAbility/HDGA_StratagemInputMode.h"
#include "GameAbility/Define/HDGA_Number.h"
#include "Controller/HDPlayerController.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"

AHDGASCharacterPlayer::AHDGASCharacterPlayer()
{
    PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* AHDGASCharacterPlayer::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHDGASCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    AHDGASPlayerState* GASPlayerState = GetPlayerState<AHDGASPlayerState>();
    NULL_CHECK(GASPlayerState);

    AbilitySystemComponent = GASPlayerState->GetAbilitySystemComponent();
    NULL_CHECK(AbilitySystemComponent);

    AbilitySystemComponent->InitAbilityActorInfo(GASPlayerState, this);

    for (const TSubclassOf<UGameplayAbility>& StartAbility : StartAbilities)
    {
        AbilitySystemComponent->GiveAbility(StartAbility);
    }

    SetupGASInputComponent();

    FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
    EffectContextHandle.AddSourceObject(this);
    FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitStatEffect, static_cast<int>(ArmorType), EffectContextHandle);

    AHDPlayerController* HDPlayerController = CastChecked<AHDPlayerController>(NewController);
    HDPlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
    HDPlayerController->CreateHUDWidget(AbilitySystemComponent);

    if(EventCallTags.Num() != EventActions.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("EventCallTags Num and TaggedEventActionList Num is Not Equal"));
    }

    for(const FGameplayTag& EventCallTag : EventCallTags.GetGameplayTagArray())
    {
        AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(EventCallTag).AddUObject(this, &AHDGASCharacterPlayer::HandleGameplayEvent);
    }

    EventActions
}

void AHDGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    SetupGASInputComponent();
}

void AHDGASCharacterPlayer::SetupGASInputComponent()
{
    VALID_CHECK(AbilitySystemComponent);
    VALID_CHECK(InputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
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

void AHDGASCharacterPlayer::GASInputPressed(FGameplayTag Tag)
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

void AHDGASCharacterPlayer::GASInputReleased(FGameplayTag Tag)
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
    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(GA_NUM_STRATAGEM_INPUTMODE);
    if (Spec->IsActive())
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
    else
    {
        ensureMsgf(Spec, TEXT("StratagemInputMode AbilitySpec is Deactive"));
    }
}

void AHDGASCharacterPlayer::HandleGameplayEvent(const FGameplayEventData* Payload)
{
    if (Payload->EventTag.MatchesTagExact(HDTAG_EVENT_STRATAGEMHUD_APPEAR))
    {
        AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
        NULL_CHECK(PlayerController);
        if (PlayerController->IsLocalController())
        {
            PlayerController->SetStratagemHUDAppear(true);
        }
    }
    else if (Payload->EventTag.MatchesTagExact(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR))
    {
        AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
        NULL_CHECK(PlayerController);
        if (PlayerController->IsLocalController())
        {
            PlayerController->SetStratagemHUDAppear(false);
        }
    }
}
