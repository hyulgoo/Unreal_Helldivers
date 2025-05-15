// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/HDCharacterNonPlayer.h"
#include "Engine/AssetManager.h"

AHDCharacterNonPlayer::AHDCharacterNonPlayer()
{
    GetMesh()->SetHiddenInGame(true);
}

void AHDCharacterNonPlayer::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AHDCharacterNonPlayer::SetDead()
{
    Super::SetDead();

    FTimerHandle DeadTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
        [&]() 
        { 
            Destroy(); 
        }
    ), DeadEventDelayTime, false);
}
