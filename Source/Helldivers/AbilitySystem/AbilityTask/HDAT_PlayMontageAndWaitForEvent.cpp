// Fill out your copyright notice in the Description page of Project Settings.

#include "HDAT_PlayMontageAndWaitForEvent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Define/HDDefine.h"

UHDAT_PlayMontageAndWaitForEvent* UHDAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitEvent(UGameplayAbility* OwningAbility, UAnimMontage* MontageToPlay, const FGameplayTagContainer EventTagContainer, float Rate, FName StartSection, const bool OnlyTriggerOnce, bool OnlyMatchExact)
{
    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

    UHDAT_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UHDAT_PlayMontageAndWaitForEvent>(OwningAbility, "PlayMontageAndWaitEvent");
    MyObj->MontageToPlay = MontageToPlay;
    MyObj->EventTags = EventTagContainer;
    MyObj->Rate = Rate;
    MyObj->StartSection = StartSection;
    MyObj->AnimRootMotionTranslationScale = 1.f;
    MyObj->bStopWhenAbilityEnds = false;
    MyObj->bOnlyMatchExact = OnlyMatchExact;
    MyObj->bOnlyTriggerOnce = OnlyTriggerOnce;

    return MyObj;
}

void UHDAT_PlayMontageAndWaitForEvent::Activate()
{
    NULL_CHECK(Ability);

    bool bPlayedMontage = false;

    if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if (AnimInstance != nullptr)
        {
            if (bOnlyMatchExact)
            {
                for (const FGameplayTag& Tag : EventTags)
                {
                    EventHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag).AddUObject(this, &UHDAT_PlayMontageAndWaitForEvent::GameplayEventCallback);
                }
            }
            else
            {
                EventHandle = ASC->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UHDAT_PlayMontageAndWaitForEvent::GameplayEventContainerCallback));
            }

            if (ASC->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection))
            {
                CONDITION_CHECK(ShouldBroadcastAbilityTaskDelegates());

                BlendedInDelegate.BindUObject(this, &UHDAT_PlayMontageAndWaitForEvent::OnMontageBlendedIn);
                AnimInstance->Montage_SetBlendedInDelegate(BlendedInDelegate, MontageToPlay);

                BlendingOutDelegate.BindUObject(this, &UHDAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
                AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

                MontageEndedDelegate.BindUObject(this, &UHDAT_PlayMontageAndWaitForEvent::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

                ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
                if (Character && (Character->GetLocalRole() == ROLE_Authority ||
                    (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
                {
                    Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
                }

                bPlayedMontage = true;
            }
        }
        else
        {
            LOG(TEXT("UAbilityTask_PlayMontageAndWait call to PlayMontage failed!"));
        }
    }
    else
    {
        LOG(TEXT("UAbilityTask_PlayMontageAndWait called on invalid AbilitySystemComponent"));
    }

    if (!bPlayedMontage)
    {
        LOG(TEXT("UAbilityTask_PlayMontageAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCancelled.Broadcast(nullptr, FGameplayEventData());
        }
    }

    SetWaitingOnAvatar();
}

void UHDAT_PlayMontageAndWaitForEvent::ExternalCancel()
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnCancelled.Broadcast(nullptr, FGameplayEventData());
    }

    Super::ExternalCancel();
}

void UHDAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
    if (Ability)
    {
        if (AbilityEnded && bStopWhenAbilityEnds)
        {
            StopPlayingMontage();
        }

        UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
        if(ASC && EventHandle.IsValid())
        {
            if (bOnlyMatchExact)
            {
                for(const FGameplayTag& Tag : EventTags)
                {
                    ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag).Remove(EventHandle);
                }
            }
            else
            {
                ASC->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
            }
        }
    }

    Super::OnDestroy(AbilityEnded);
}

void UHDAT_PlayMontageAndWaitForEvent::OnMontageBlendedIn(UAnimMontage* Montage)
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnBlendedIn.Broadcast(nullptr, FGameplayEventData());
    }
}

void UHDAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
    const bool bPlayingThisMontage = (Montage == MontageToPlay) && Ability && Ability->GetCurrentMontage() == MontageToPlay;

    if (bPlayingThisMontage && (bInterrupted || !bAllowInterruptAfterBlendOut))
    {
        if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
        {
            ASC->ClearAnimatingAbility(Ability);
        }
    }

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        if (bInterrupted)
        {
            bAllowInterruptAfterBlendOut = false;
            EndTask();
        }
        else
        {
            OnBlendOut.Broadcast(nullptr, FGameplayEventData());
        }
    }
}

void UHDAT_PlayMontageAndWaitForEvent::OnGameplayAbilityCancelled()
{
    EndTask();
}

void UHDAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCompleted.Broadcast(nullptr, FGameplayEventData());
        }
    }

    EndTask();
}

void UHDAT_PlayMontageAndWaitForEvent::GameplayEventCallback(const FGameplayEventData* Payload)
{
    if(Payload)
    {
        if (EventTags.HasTag(Payload->EventTag))
        {
            GameplayEventContainerCallback(Payload->EventTag, Payload);
        }
    }
}

void UHDAT_PlayMontageAndWaitForEvent::GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload)
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ensureMsgf(Payload, TEXT("GameplayEventCallback expected non-null Payload"));
        FGameplayEventData TempPayload = Payload ? *Payload : FGameplayEventData{};
        TempPayload.EventTag = MatchingTag;
        OnEventReceived.Broadcast(nullptr, MoveTemp(TempPayload));
    }

    if (bOnlyTriggerOnce)
    {
        EndTask();
    }
}

bool UHDAT_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
    NULL_CHECK_WITH_RETURNTYPE(Ability, false);

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    NULL_CHECK_WITH_RETURNTYPE(ActorInfo, false);

    UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
    NULL_CHECK_WITH_RETURNTYPE(AnimInstance, false);

    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (ASC && Ability)
    {
        if (ASC->GetAnimatingAbility() == Ability
            && ASC->GetCurrentMontage() == MontageToPlay)
        {
            // Unbind delegates so they don't get called as well
            FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
            if (MontageInstance)
            {
                MontageInstance->OnMontageBlendedInEnded.Unbind();
                MontageInstance->OnMontageBlendingOutStarted.Unbind();
                MontageInstance->OnMontageEnded.Unbind();
            }

            ASC->CurrentMontageStop();
            return true;
        }
    }

    return false;
}
