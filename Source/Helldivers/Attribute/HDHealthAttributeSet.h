// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HDHealthAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class HELLDIVERS_API UHDHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	explicit UHDHealthAttributeSet();

	ATTRIBUTE_ACCESSORS(UHDHealthAttributeSet, CurrentHealth);
	ATTRIBUTE_ACCESSORS(UHDHealthAttributeSet, MaxHealth);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = CurrentHealth, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CurrentHealth;
	
	UPROPERTY(BlueprintReadOnly, Category = CurrentHealth, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;
};
