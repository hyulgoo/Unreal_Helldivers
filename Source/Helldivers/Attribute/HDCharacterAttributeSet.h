// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HDCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class HELLDIVERS_API UHDCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	explicit UHDCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, CurrentHealth);
	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, CurrentSpeed);
	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, CrawlingSpeed);
	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, CrouchSpeed);
	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, WalkSpeed);
	ATTRIBUTE_ACCESSORS(UHDCharacterAttributeSet, SprintSpeed);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
protected:	
	UPROPERTY(BlueprintReadOnly, Category = CurrentHealth, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = CurrentHealth, Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;
	
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
