
#include "GameplayAbilityHelper.h"
#include "Define/HDDefine.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const FGameplayAbilityActorInfo* TargetActorInfo, const float Magnitude)
{
    CONDITION_CHECK(EventTag.IsValid());

    AActor* SourceActor = SourceActorInfo->AvatarActor.Get();
    VALID_CHECK(SourceActor);

    AActor* TargetActor = TargetActorInfo->AvatarActor.Get();
    VALID_CHECK(TargetActor);

    UAbilitySystemComponent* TargetASC = TargetActorInfo->AbilitySystemComponent.Get();
    VALID_CHECK(TargetASC);

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = SourceActor;
    EventData.Target = TargetActor;
    EventData.EventMagnitude = Magnitude;

    TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, UAbilitySystemComponent* TargetASC, const float Magnitude /*= 0.f*/)
{
    VALID_CHECK(TargetASC);
    CONDITION_CHECK(EventTag.IsValid());

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = Instigator;
    EventData.Target = TargetASC->GetOwnerActor();
    EventData.EventMagnitude = Magnitude;

    TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const float Magnitude /*= 0.f*/)
{
    NULL_CHECK(SourceActorInfo->AvatarActor);
    CONDITION_CHECK(EventTag.IsValid());

    AActor* OwningActor = SourceActorInfo->AvatarActor.Get();
    VALID_CHECK(OwningActor);

    UAbilitySystemComponent* SourceASC = SourceActorInfo->AbilitySystemComponent.Get();
    VALID_CHECK(SourceASC);

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = OwningActor;
    EventData.Target = OwningActor;
    EventData.EventMagnitude = Magnitude;

    SourceASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, UAbilitySystemComponent* SourceASC, const float Magnitude /*= 0.f*/)
{
    VALID_CHECK(SourceASC);
    CONDITION_CHECK(EventTag.IsValid());

    AActor* OwningActor = SourceASC->GetOwnerActor();
    VALID_CHECK(OwningActor);

    FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = OwningActor;
	EventData.Target = OwningActor;
    EventData.EventMagnitude = Magnitude;

    SourceASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

UAbilitySystemComponent* FGameplayAbilityHelper::GetAbilitySystemComponentFromActor(AActor* Actor)
{
    NULL_CHECK_WITH_RETURNTYPE(Actor, nullptr);

    return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
}

const bool FGameplayAbilityHelper::HasMatchingTagGameplayTag(UAbilitySystemComponent* TargetASC, const FGameplayTag TagToCheck)
{
    VALID_CHECK_WITH_RETURNTYPE(TargetASC, false);
    CONDITION_CHECK_WITH_RETURNTYPE(TagToCheck.IsValid(), false);

    return TargetASC->HasMatchingGameplayTag(TagToCheck);
}

void FGameplayAbilityHelper::AddTagToTarget(UAbilitySystemComponent* TargetASC, const FGameplayTag Tag)
{
    VALID_CHECK(TargetASC);
    CONDITION_CHECK(Tag.IsValid());

    TargetASC->AddLooseGameplayTag(Tag);
}

void FGameplayAbilityHelper::ExcuteGameplayCue(const FGameplayTag Tag, const FGameplayTagContainer TagContainer, const FVector& Location, const FVector Normal, UAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);
    CONDITION_CHECK(Tag.IsValid());

    AActor* OwningActor = ASC->GetOwnerActor();
    VALID_CHECK(OwningActor);

    FGameplayCueParameters Params;
    Params.Location = Location;
    Params.Normal = Normal;
    Params.Instigator = OwningActor;
    Params.SourceObject = OwningActor;
    Params.AggregatedSourceTags = TagContainer;

    ASC->ExecuteGameplayCue(Tag, Params);
}

void FGameplayAbilityHelper::RemoveActiveEffectByGrantedTag(const FGameplayTag Tag, UAbilitySystemComponent* ASC)
{
    VALID_CHECK(ASC);
    CONDITION_CHECK(Tag.IsValid());

    FGameplayTagContainer GrantedTags(Tag);
    FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(GrantedTags);

    ASC->RemoveActiveEffects(Query);
}
