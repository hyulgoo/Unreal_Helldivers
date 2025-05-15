// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "HDBaseAttributeSet.generated.h"

UCLASS()
class HELLDIVERS_API UHDBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	explicit UHDBaseAttributeSet() = default;

protected:
    virtual const bool CheckAttributeNum(const int Num) const PURE_VIRTUAL(UHDBaseAttributeSet::CheckAttributeNum, return {};);

	const int32 GetAttributeNum(UClass* AttributeSetClass) const;
};
