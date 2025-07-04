// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/HDCharacterNonPlayer.h"
#include "Engine/AssetManager.h"

AHDCharacterNonPlayer::AHDCharacterNonPlayer()
{
    UHDAbilitySystemComponent* ASC = CreateDefaultSubobject<UHDAbilitySystemComponent>("HDAbilitySystem");
    SetAbilitySystemComponent(this, Cast<UHDAbilitySystemComponent>(ASC));
}

void AHDCharacterNonPlayer::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AHDCharacterNonPlayer::SetDead()
{
    Super::SetDead();

	const float RandomDeadEventDelayTime = FMath::FRandRange(DeadEventDelayTime, DeadEventDelayTime * 1.5f);
    FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
    ), RandomDeadEventDelayTime, false);
}
