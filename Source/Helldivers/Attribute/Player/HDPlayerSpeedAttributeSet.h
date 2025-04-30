// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HDPlayerSpeedAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class HELLDIVERS_API UHDPlayerSpeedAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	explicit UHDPlayerSpeedAttributeSet();

	ATTRIBUTE_ACCESSORS(UHDPlayerSpeedAttributeSet, CurrentSpeed);
	ATTRIBUTE_ACCESSORS(UHDPlayerSpeedAttributeSet, CrawlingSpeed);
	ATTRIBUTE_ACCESSORS(UHDPlayerSpeedAttributeSet, CrouchSpeed);
	ATTRIBUTE_ACCESSORS(UHDPlayerSpeedAttributeSet, WalkSpeed);
	ATTRIBUTE_ACCESSORS(UHDPlayerSpeedAttributeSet, SprintSpeed);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = CurrentSpeed, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CurrentSpeed;
		
    UPROPERTY(BlueprintReadOnly, Category = CurrentSpeed, Meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData CrawlingSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category = CurrentSpeed, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CrouchSpeed;

	UPROPERTY(BlueprintReadOnly, Category = CurrentSpeed, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData WalkSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category = CurrentSpeed, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData SprintSpeed;
};
