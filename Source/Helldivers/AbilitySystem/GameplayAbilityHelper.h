#pragma once

#include "Abilities/GameplayAbility.h"

class FGameplayAbilityHelper
{
public:
	static void			SendGameplayEventToTarget(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const FGameplayAbilityActorInfo* TargetActorInfo, const float Magnitude = 0.f);
	static void			SendGameplayEventToTarget(const FGameplayTag EventTag, AActor*Instigator ,UAbilitySystemComponent* TargetASC, const float Magnitude = 0.f);
	static void			SendGameplayEventToSelf(const FGameplayTag EventTag, const FGameplayAbilityActorInfo* SourceActorInfo, const float Magnitude = 0.f);
	static void			SendGameplayEventToSelf(const FGameplayTag EventTag, UAbilitySystemComponent* SourceASC, const float Magnitude = 0.f);

	static const bool	HasMatchingTagGameplayTag(UAbilitySystemComponent* TargetASC, const FGameplayTag TagToCheck);
	static void			AddTagToTarget(UAbilitySystemComponent* TargetASC, const FGameplayTag Tag);

    static void         ExcuteGameplayCue(const FGameplayTag Tag, const FGameplayTagContainer TagContainer, const FVector& Location, const FVector Normal, UAbilitySystemComponent* ASC);

    static void         RemoveActiveEffectByGrantedTag(const FGameplayTag Tag, UAbilitySystemComponent* ASC);
};

