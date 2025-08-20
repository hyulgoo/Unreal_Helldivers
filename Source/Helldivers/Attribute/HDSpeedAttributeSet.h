// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HDSpeedAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class HELLDIVERS_API UHDSpeedAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, CrawlingSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, CrouchSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, WalkSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, SprintSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxStamina);

	virtual void            PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    //virtual void            PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    //virtual bool            PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
    //virtual void            PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
public:
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData CurrentSpeed;	
											
	UPROPERTY(BlueprintReadOnly)
    FGameplayAttributeData CrawlingSpeed;
										
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData CrouchSpeed;	
										
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData WalkSpeed;	
										
	UPROPERTY(BlueprintReadOnly)
    FGameplayAttributeData SprintSpeed;	
										
	UPROPERTY(BlueprintReadOnly)
    FGameplayAttributeData CurrentStamina;

	UPROPERTY(BlueprintReadOnly)
    FGameplayAttributeData MaxStamina;
};
