// Fill out your copyright notice in the Description page of Project Settings.

#include "HDProjectileAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UHDProjectileAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetSpeedAttribute())
	{
		NewValue = NewValue <= 0.f ? 0.f : NewValue;
		UProjectileMovementComponent* ProjectileMovementComponent = GetOwningActor()->GetComponentByClass<UProjectileMovementComponent>();;
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = NewValue;
			ProjectileMovementComponent->MaxSpeed = NewValue;
		}
	}
}

bool UHDProjectileAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
	{
		return false;
	}

	return true;
}

void UHDProjectileAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
