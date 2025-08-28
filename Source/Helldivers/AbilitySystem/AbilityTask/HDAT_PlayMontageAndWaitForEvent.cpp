// Fill out your copyright notice in the Description page of Project Settings.

#include "HDAT_PlayMontageAndWaitForEvent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Define/HDDefine.h"

UHDAT_PlayMontageAndWaitForEvent* UHDAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitEvent(UGameplayAbility* OwningAbility, UAnimMontage* MontageToPlay, const FGameplayTagContainer EventTagContainer, float Rate, FName StartSection, const bool EndTaskOnMontageEnded, const bool OnlyTriggerOnce, bool OnlyMatchExact)
{
    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

    UHDAT_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UHDAT_PlayMontageAndWaitForEvent>(OwningAbility, "PlayMontageAndWaitEvent");
    MyObj->MontageToPlay = MontageToPlay;
    MyObj->EventTags = EventTagContainer;
    MyObj->Rate = Rate;
    MyObj->StartSection = StartSection;
    MyObj->AnimRootMotionTranslationScale = 1.f;
    MyObj->bStopWhenAbilityEnds = true;
    MyObj->bEndTaskOnMontageEnded = EndTaskOnMontageEnded;
    MyObj->bOnlyMatchExact = OnlyMatchExact;
    MyObj->bOnlyTriggerOnce = OnlyTriggerOnce;

    return MyObj;
}

void UHDAT_PlayMontageAndWaitForEvent::Activate()
{
    Super::Activate();

    NULL_CHECK(Ability);

    if (AbilitySystemComponent.IsValid())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if (AnimInstance)
        {
            if (bOnlyMatchExact)
            {
                for (const FGameplayTag& Tag : EventTags)
                {
                    EventHandle = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(Tag).AddUObject(this, &UHDAT_PlayMontageAndWaitForEvent::GameplayEventCallback);
                }
            }
            else
            {
                EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UHDAT_PlayMontageAndWaitForEvent::GameplayEventContainerCallback));
            }

            if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection))
            {
                CONDITION_CHECK(ShouldBroadcastAbilityTaskDelegates());

                MontageEndedDelegate.BindUObject(this, &UHDAT_PlayMontageAndWaitForEvent::OnMontageEnded);
                if (bEndTaskOnMontageEnded)
                {
                    AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
                }

                ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
                if (Character && (Character->GetLocalRole() == ROLE_Authority ||
                    (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
                {
                    Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
                }
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

void UHDAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCompleted.Broadcast(FGameplayEventData());
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
        OnEventReceived.Broadcast(MoveTemp(TempPayload));
    }

    if (bOnlyTriggerOnce)
    {
        EndTask();
    }
}

void UHDAT_PlayMontageAndWaitForEvent::JumpToSection(const FName SectionName)
{
    if (Ability && AbilitySystemComponent.IsValid())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if (AnimInstance)
        {
            AbilitySystemComponent->CurrentMontageJumpToSection(SectionName);
        }
    }
}

void UHDAT_PlayMontageAndWaitForEvent::ResetMontage()
{
    if(Ability)
    {
        AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection);
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
    if (ASC)
    {
        if (ASC->GetAnimatingAbility() == Ability && ASC->GetCurrentMontage() == MontageToPlay)
        {
            // Unbind delegates so they don't get called as well
            FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
            if (MontageInstance)
            {
                MontageInstance->OnMontageEnded.Unbind();
            }

            ASC->CurrentMontageStop();
            return true;
        }
    }

    return false;
}
