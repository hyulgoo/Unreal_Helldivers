// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/HDPlayerController.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Component/HDStratagemComponent.h"
#include "Component/HDCombatComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "UI/HDGASPlayerUserWidget.h"
#include "UI/HDStratagemHUDUserWidget.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Weapon/HDWeapon.h"

AHDPlayerController::AHDPlayerController()
    : PossessPawnASC(nullptr)
    , PlayerHUDWidgetClass(nullptr)
    , PlayerHUDWidget(nullptr)
    , StratagemHUDWidgetClass(nullptr)
    , StratagemHUDWidget(nullptr)
{
}

void AHDPlayerController::BeginPlay()
{
    Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void AHDPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    TScriptInterface<IAbilitySystemInterface> AbilityInterface = aPawn;
    if (AbilityInterface)
    {
        PossessPawnASC = AbilityInterface->GetAbilitySystemComponent();
        SetPossessAbilitySystemComponentBindEventCall(PossessPawnASC);

        CreateHUDWidget(aPawn);
	}
	else
	{
		PossessPawnASC = nullptr;
	}

    ConsoleCommand(TEXT("showdebug abilitysystem"));
}

void AHDPlayerController::SetPossessAbilitySystemComponentBindEventCall(UAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);

    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_PLAYERHUD_AMMO).AddUObject(this, &AHDPlayerController::OnPlayerHUDInfoChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_PLAYERHUD_CAPACITY).AddUObject(this, &AHDPlayerController::OnPlayerHUDInfoChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND).AddUObject(this, &AHDPlayerController::OnStratagemHUDInfoChanged);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_APPEAR).AddUObject(this, &AHDPlayerController::StratagemHUDAppear);
    ASC->GenericGameplayEventCallbacks.FindOrAdd(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR).AddUObject(this, &AHDPlayerController::StratagemHUDAppear);

}

void AHDPlayerController::OnPlayerHUDInfoChanged(const FGameplayEventData* Payload)
{
    if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_AMMO)
    {
        const int32 NewAmmoCount = static_cast<int32>(Payload->EventMagnitude);
        ChangeAmmoHUDInfo(NewAmmoCount);
    }
    else if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_CAPACITY)
    {
        const int32 NewCapacityCount = static_cast<int32>(Payload->EventMagnitude);
        ChangeCapacityHUDInfo(NewCapacityCount);
    }
}

void AHDPlayerController::OnStratagemHUDInfoChanged(const FGameplayEventData* Payload)
{
    if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND)
    {
        UHDStratagemComponent* StratagemComponent = Payload->Instigator->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);
        
        const TArray<FName> CommandMatchStratagemNameList = StratagemComponent->GetCommandMatchStratagemNameList();
        const int32 CurrentInputNum = StratagemComponent->GetCurrentInputNum();
        
        SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputNum);
    }
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
        PlayerHUDWidget->SetAbilitySystemComponent(PossessPawnASC);
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

void AHDPlayerController::ChangeAmmoHUDInfo(const int32 NewAmmoCount)
{
    VALID_CHECK(PlayerHUDWidget);
    PlayerHUDWidget->OnAmmoCountChanged(NewAmmoCount);
}

void AHDPlayerController::ChangeCapacityHUDInfo(const int32 NewCapacityCount)
{
    VALID_CHECK(PlayerHUDWidget);
    PlayerHUDWidget->OnCapacityCountChanged(NewCapacityCount);
}

void AHDPlayerController::SetHUDActiveByCurrentInputMatchList(const TArray<FName>& MatchStratagemList, const int32 CurrentInputNum)
{
    VALID_CHECK(StratagemHUDWidget);
    StratagemHUDWidget->SetHUDActiveByCurrentInputMatchList(MatchStratagemList, CurrentInputNum);
}

void AHDPlayerController::StratagemHUDAppear(const FGameplayEventData* Payload)
{
    VALID_CHECK(StratagemHUDWidget);
    if (Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_APPEAR)
    {
        StratagemHUDWidget->WidgetAppear(true);
    }
    else if(Payload->EventTag == HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR)
    {
        StratagemHUDWidget->WidgetAppear(false);
    }
}
