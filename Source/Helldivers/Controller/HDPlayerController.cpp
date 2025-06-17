// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/HDPlayerController.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Component/Character/HDStratagemComponent.h"
#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "UI/HDGASPlayerUserWidget.h"
#include "UI/HDStratagemHUDUserWidget.h"
#include "Weapon/HDWeapon.h"

AHDPlayerController::AHDPlayerController()
    : PossessPawnAbilitySystemComponent(nullptr)
    , PlayerHUDWidgetClass(nullptr)
    , PlayerHUDWidget(nullptr)
    , StratagemHUDWidgetClass(nullptr)
    , StratagemHUDWidget(nullptr)
{
}

void AHDPlayerController::SetPossessPawnAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);

    PossessPawnAbilitySystemComponent = ASC;
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

    ConsoleCommand(TEXT("showdebug abilitysystem"));
}

void AHDPlayerController::OnPlayerHUDInfoChanged(const FGameplayEventData& Data)
{
    if (Data.EventTag == HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR)
    {

    }
    else if (Data.EventTag == HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR)
    {

    }
}

void AHDPlayerController::OnStratagemHUDInfoChanged(const FGameplayEventData& Data)
{
    if (Data.EventTag == HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR)
    {
        UHDStratagemComponent* StratagemComponent = Data.Instigator->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);
        
        const TArray<FName> CommandMatchStratagemNameList = StratagemComponent->GetCommandMatchStratagemNameList();
        const int32 CurrentInputNum = StratagemComponent->GetCurrentInputNum();
        
        SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputNum);
    }
}

void AHDPlayerController::CreateHUDWidget(ACharacter* PlayerCharacter)
{
    NULL_CHECK(PlayerCharacter);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    if(PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UHDGASPlayerUserWidget>(World, PlayerHUDWidgetClass, FName("PlayerHUDWidget"));
        NULL_CHECK(PlayerHUDWidget);

        PlayerHUDWidget->SetAbilitySystemComponentByOwningCharacter(PlayerCharacter);
        PlayerHUDWidget->AddToViewport();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerHUDWidgetClass is nullptr!"));
    }

    if(StratagemHUDWidgetClass)
    {
        StratagemHUDWidget = CreateWidget<UHDStratagemHUDUserWidget>(World, StratagemHUDWidgetClass, FName("StratagemHUDWidget"));
        NULL_CHECK(StratagemHUDWidget);

        UHDStratagemComponent* StratagemComponent = PlayerCharacter->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);

        StratagemHUDWidget->SetStratagemListHUD(StratagemComponent->GetAvaliableStratagemDataTable());
        StratagemHUDWidget->AddToViewport();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("StratagemHUDWidgetClass is nullptr!"));
    }
}

void AHDPlayerController::SetWeaponHUDInfo(AHDWeapon* NewWeapon)
{
	VALID_CHECK(NewWeapon);

    AActor* WeaponOwner = NewWeapon->GetOwner();
    NULL_CHECK(WeaponOwner);

    AHDGASCharacterPlayer* GASPlayer = Cast<AHDGASCharacterPlayer>(WeaponOwner);
    NULL_CHECK(GASPlayer);

    CreateHUDWidget(GASPlayer);

    VALID_CHECK(PlayerHUDWidget);

    PlayerHUDWidget->SetChangedWeaponAmmoCountInfo(NewWeapon->GetAmmoCount(), NewWeapon->GetMaxAmmoCount());
    PlayerHUDWidget->SetChangedWeaponCapacityCountInfo(NewWeapon->GetCapacityCount(), NewWeapon->GetMaxCapacityCount());

    // TODO Grenade HUD
    /*AHDCharacterPlayer* CharacterPlayer = Cast<AHDCharacterPlayer>(GetPawn());
    NULL_CHECK(CharacterPlayer);
    PlayerHUDWidget->OnGrenadeCountChanged(CharacterPlayer->GetGrenade);*/
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

void AHDPlayerController::SetStratagemHUDAppear(const bool bAppear)
{
    VALID_CHECK(StratagemHUDWidget);
    StratagemHUDWidget->WidgetAppear(bAppear);
}
