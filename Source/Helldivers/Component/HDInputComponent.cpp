// Fill out your copyright notice in the Description page of Project Settings.

#include "HDInputComponent.h"

const TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>>& UHDInputComponent::GetInputActionMap() const
{
    return InputActionMap;
}
