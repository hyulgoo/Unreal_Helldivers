// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HDProjectileAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class HELLDIVERS_API UHDProjectileAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	ATTRIBUTE_ACCESSORS(UHDProjectileAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UHDProjectileAttributeSet, Speed);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:	
	UPROPERTY(BlueprintReadOnly, Category = Health, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = Speed, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Speed;
};
