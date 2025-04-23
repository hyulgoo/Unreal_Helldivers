// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/AbilityTask/HDAT_JumpAndWaitForLanding.h"
#include "GameFramework/Character.h"

UHDAT_JumpAndWaitForLanding* UHDAT_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
    return NewAbilityTask<UHDAT_JumpAndWaitForLanding>(OwningAbility);
}

void UHDAT_JumpAndWaitForLanding::Activate()
{
	Super::Activate();

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.AddDynamic(this, &UHDAT_JumpAndWaitForLanding::OnLandedCallback);
	Character->Jump();

	SetWaitingOnAvatar();
}

void UHDAT_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.RemoveDynamic(this, &UHDAT_JumpAndWaitForLanding::OnLandedCallback);

	Super::OnDestroy(AbilityEnded);
}

void UHDAT_JumpAndWaitForLanding::OnLandedCallback(const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast();
	}
}
