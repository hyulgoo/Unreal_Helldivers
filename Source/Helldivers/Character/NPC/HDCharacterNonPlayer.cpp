// Fill out your copyright notice in the Description page of Project Settings.

#include "HDCharacterNonPlayer.h"
#include "Component/HDAbilitySystemComponent.h"

void AHDCharacterNonPlayer::BeginPlay()
{
    Super::BeginPlay();

    UHDAbilitySystemComponent* ASC = CreateDefaultSubobject<UHDAbilitySystemComponent>("HDAbilitySystem");
    SetAbilitySystemComponent(this, ASC);
}

void AHDCharacterNonPlayer::SetDead()
{
    Super::SetDead();

	const float RandomDeadEventDelayTime = FMath::FRandRange(DeadEventDelayTime, DeadEventDelayTime * 1.5f);
    FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() { Destroy(); } ), RandomDeadEventDelayTime, false);
}
