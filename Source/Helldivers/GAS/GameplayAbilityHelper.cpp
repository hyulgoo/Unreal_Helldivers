#include "GameplayAbilityHelper.h"
#include "Define/HDDefine.h"
#include "AbilitySystemComponent.h"

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const FGameplayAbilityActorInfo* TargetActorInfo, const float Magnitude)
{
    AActor* SourceActor = SourceActorInfo->AvatarActor.Get();
    VALID_CHECK(SourceActor);

    AActor* TargetActor = TargetActorInfo->AvatarActor.Get();
    VALID_CHECK(TargetActor);

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = SourceActor;
    EventData.Target = TargetActor;
    EventData.EventMagnitude = Magnitude;

    UAbilitySystemComponent* TargetASC = TargetActorInfo->AbilitySystemComponent.Get();
    VALID_CHECK(TargetASC);
    TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, UAbilitySystemComponent* TargetASC, const float Magnitude /*= 0.f*/)
{
    VALID_CHECK(TargetASC);

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = Instigator;
    EventData.Target = TargetASC->GetOwnerActor();
    EventData.EventMagnitude = Magnitude;

    TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const float Magnitude /*= 0.f*/)
{
    AActor* OwningActor = SourceActorInfo->AvatarActor.Get();
    VALID_CHECK(OwningActor);

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = OwningActor;
    EventData.Target = OwningActor;
    EventData.EventMagnitude = Magnitude;

    UAbilitySystemComponent* SourceASC = SourceActorInfo->AbilitySystemComponent.Get();
    VALID_CHECK(SourceASC);
    SourceASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, UAbilitySystemComponent* SourceASC, const float Magnitude /*= 0.f*/)
{
    VALID_CHECK(SourceASC);

    AActor* OwningActor = SourceASC->GetOwnerActor();
    VALID_CHECK(OwningActor);

    FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = OwningActor;
	EventData.Target = OwningActor;
    EventData.EventMagnitude = Magnitude;

    SourceASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
