// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/HDPlayerController.h"
#include "Define/HDDefine.h"
#include "UI/HDGASPlayerUserWidget.h"
#include "UI/HDStratagemHUDUserWidget.h"
#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "Weapon/HDWeapon.h"

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

void AHDPlayerController::CreateHUDWidget(UAbilitySystemComponent* NewAbilitySystemComponent)
{
    UWorld* World = GetWorld();
    VALID_CHECK(World);

    if(PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UHDGASPlayerUserWidget>(World, PlayerHUDWidgetClass, FName("PlayerHUDWidget"));
        NULL_CHECK(PlayerHUDWidget);

        PlayerHUDWidget->SetAbilitySystemComponent(NewAbilitySystemComponent);
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

        AHDCharacterPlayer* CharacterPlayer = Cast<AHDCharacterPlayer>(GetPawn());
        NULL_CHECK(CharacterPlayer);

        StratagemHUDWidget->SetStratagemListHUD(CharacterPlayer->GetAvaliableStratagemDataTable());
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

    UAbilitySystemComponent* AbilitySystemComponent = GASPlayer->GetAbilitySystemComponent();
    NULL_CHECK(AbilitySystemComponent);

    CreateHUDWidget(AbilitySystemComponent);

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
