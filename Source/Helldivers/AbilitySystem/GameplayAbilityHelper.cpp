
#include "GameplayAbilityHelper.h"
#include "Define/HDDefine.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const FGameplayAbilityActorInfo* TargetActorInfo, const float Magnitude)
{
    CONDITION_CHECK(EventTag.IsValid());

    AActor* SourceActor = SourceActorInfo->AvatarActor.Get();
    VALID_CHECK(SourceActor);

    AActor* TargetActor = TargetActorInfo->AvatarActor.Get();
    VALID_CHECK(TargetActor);

    UAbilitySystemComponent* TargetASC = TargetActorInfo->AbilitySystemComponent.Get();
    VALID_CHECK(TargetASC);

    SendGameplayEventToTarget(EventTag, SourceActor, TargetActor, Magnitude);
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

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, AActor* TargetActor, const float Magnitude /*= 0.f*/)
{
    SendGameplayEventToTarget(EventTag, Instigator, GetAbilitySystemComponentFromActor(TargetActor), Magnitude);
}

void FGameplayAbilityHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, AActor* TargetActor, const FGameplayEventData& Payload)
{
    UAbilitySystemComponent* TargetASC = GetAbilitySystemComponentFromActor(TargetActor);
    NULL_CHECK(TargetASC);

    TargetASC->HandleGameplayEvent(EventTag, &Payload);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const float Magnitude /*= 0.f*/)
{
    NULL_CHECK(SourceActorInfo->AvatarActor);

    AActor* OwningActor = SourceActorInfo->AvatarActor.Get();
    VALID_CHECK(OwningActor);

    SendGameplayEventToSelf(EventTag, OwningActor, Magnitude);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, UAbilitySystemComponent* SourceASC, const float Magnitude /*= 0.f*/)
{
    AActor* OwningActor = SourceASC->GetOwnerActor();
    VALID_CHECK(OwningActor);

    SendGameplayEventToTarget(EventTag, OwningActor, SourceASC, Magnitude);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, AActor* SourceActor, const float Magnitude /*= 0.f*/)
{
    SendGameplayEventToSelf(EventTag, GetAbilitySystemComponentFromActor(SourceActor), Magnitude);
}

void FGameplayAbilityHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, AActor* SourceActor, const FGameplayEventData& Payload)
{
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActor(SourceActor);
    NULL_CHECK(SourceASC);

    SourceASC->HandleGameplayEvent(EventTag, &Payload);
}

UAbilitySystemComponent* FGameplayAbilityHelper::GetAbilitySystemComponentFromActor(AActor* Actor)
{
    NULL_CHECK_WITH_RETURNTYPE_WITHOUT_LOG(Actor, nullptr);

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
    VALID_CHECK(ASC);
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

void FGameplayAbilityHelper::ExcuteGameplayCue(const FGameplayTag Tag, const FVector& Location, const FVector Normal, UAbilitySystemComponent* ASC)
{
    ExcuteGameplayCue(Tag, FGameplayTagContainer::EmptyContainer, Location, Normal, ASC);
}

void FGameplayAbilityHelper::ExcuteGameplayCue(const FGameplayTag Tag, const FVector& Location, const FVector Normal, AActor* OwningActor)
{
    ExcuteGameplayCue(Tag, Location, Normal, GetAbilitySystemComponentFromActor(OwningActor));
}

void FGameplayAbilityHelper::RemoveActiveEffectByGrantedTag(const FGameplayTag Tag, UAbilitySystemComponent* ASC)
{
    VALID_CHECK(ASC);
    CONDITION_CHECK(Tag.IsValid());

    FGameplayTagContainer GrantedTags(Tag);
    FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(GrantedTags);

    ASC->RemoveActiveEffects(Query);
}

const FGameplayTagContainer FGameplayAbilityHelper::GetAllChildTag(const FGameplayTag ParentTag)
{
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(ParentTag);
}
