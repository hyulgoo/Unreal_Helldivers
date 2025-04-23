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
    if (IsValid(GASPlayerState))
    {
        AbilitySystemComponent = GASPlayerState->GetAbilitySystemComponent();
        AbilitySystemComponent->InitAbilityActorInfo(GASPlayerState, this);

        for (const auto& StartAbility : StartAbilities)
        {
            FGameplayAbilitySpec StartSpec(StartAbility);
            AbilitySystemComponent->GiveAbility(StartSpec);
        }

        for (const auto& StartInputAbility : StartInputAbilities)
        {
            FGameplayAbilitySpec StartSpec(StartInputAbility.Value);
            StartSpec.InputID = StartInputAbility.Key;
            AbilitySystemComponent->GiveAbility(StartSpec);
        }

        SetupGASInputComponent();

        FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
        EffectContextHandle.AddSourceObject(this);
        FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitStatEffect, static_cast<int>(ArmorType), EffectContextHandle);

        AHDPlayerController* HDPlayerController = CastChecked<AHDPlayerController>(NewController);
        //HDPlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
        HDPlayerController->CreateHUDWidget(AbilitySystemComponent);
    }    
}

void AHDGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    SetupGASInputComponent();
}

void AHDGASCharacterPlayer::SetupGASInputComponent()
{
    if (IsValid(AbilitySystemComponent) && IsValid(InputComponent))
    {
        UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
        EnhancedInputComponent->BindAction(JumpAction,          ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputPressed,    GA_NUM_JUMP);
        EnhancedInputComponent->BindAction(JumpAction,          ETriggerEvent::Completed, this, &AHDGASCharacterPlayer::GASInputReleased,   GA_NUM_JUMP);
        EnhancedInputComponent->BindAction(ShoulderAction,      ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputPressed,    GA_NUM_SHOULDER);
        EnhancedInputComponent->BindAction(ShoulderAction,      ETriggerEvent::Completed, this, &AHDGASCharacterPlayer::GASInputReleased,   GA_NUM_SHOULDER);
        EnhancedInputComponent->BindAction(FireAction,          ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputPressed,    GA_NUM_FIRE);
        EnhancedInputComponent->BindAction(FireAction,          ETriggerEvent::Completed, this, &AHDGASCharacterPlayer::GASInputReleased,   GA_NUM_FIRE);
        EnhancedInputComponent->BindAction(StratagemModeAction, ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::GASInputPressed,    GA_NUM_STRATAGEM_INPUTMODE);
        EnhancedInputComponent->BindAction(StratagemModeAction, ETriggerEvent::Completed, this, &AHDGASCharacterPlayer::GASInputReleased,   GA_NUM_STRATAGEM_INPUTMODE);
        EnhancedInputComponent->BindAction(InputCommandAction,  ETriggerEvent::Triggered, this, &AHDGASCharacterPlayer::InputStratagemCommand);
    }
}

void AHDGASCharacterPlayer::GASInputPressed(int32 InputId)
{
    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputId);
    if (Spec)
    {
        Spec->InputPressed = true;
        if (Spec->IsActive())
        {
            AbilitySystemComponent->AbilitySpecInputPressed(*Spec);
        }
        else
        {
            AbilitySystemComponent->TryActivateAbility(Spec->Handle);
        }
    }
    else
    {
        ensureMsgf(Spec, TEXT("Fail To Find AbilitySpec[%d]"), InputId);
    }
}

void AHDGASCharacterPlayer::GASInputReleased(int32 InputId)
{
    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputId);
    if(Spec)
    {
        Spec->InputPressed = false;
        if (Spec->IsActive())
        {
            AbilitySystemComponent->AbilitySpecInputReleased(*Spec);
        }
    }
    else
    {
        ensureMsgf(Spec, TEXT("Fail To Find AbilitySpec[%d]"), InputId);       
    }
}

void AHDGASCharacterPlayer::InputStratagemCommand(const FInputActionValue& Value)
{
    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(GA_NUM_STRATAGEM_INPUTMODE);
    if (Spec->IsActive())
    {
        EHDCommandInput NewCommand = EHDCommandInput::Count;
        const FVector2D Input = Value.Get<FVector2D>();
        if (Input.Y > 0.5f)
        {
            NewCommand = EHDCommandInput::Up;
        }
        else if (Input.Y < -0.5f)
        {
            NewCommand = EHDCommandInput::Down;
        }
        else if (Input.X > 0.5f)
        {
            NewCommand = EHDCommandInput::Right;
        }
        else if (Input.X < -0.5f)
        {
            NewCommand = EHDCommandInput::Left;
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
        ensureMsgf(Spec, TEXT("Fail To Find StratagemInputMode AbilitySpec"));
    }
}

void AHDGASCharacterPlayer::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
    if (EventTag.MatchesTagExact(HDTAG_EVENT_STRATAGEMHUD_APPEAR))
    {
        AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
        NULL_CHECK(PlayerController);
        if (PlayerController->IsLocalController())
        {
            PlayerController->SetStratagemHUDAppear(true);
        }
    }
    else if (EventTag.MatchesTagExact(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR))
    {
        AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
        NULL_CHECK(PlayerController);
        if (PlayerController->IsLocalController())
        {
            PlayerController->SetStratagemHUDAppear(false);
        }
    }
}
