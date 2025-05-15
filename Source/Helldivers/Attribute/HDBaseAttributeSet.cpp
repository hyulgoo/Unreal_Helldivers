// Fill out your copyright notice in the Description page of Project Settings.

#include "Attribute/HDBaseAttributeSet.h"

const int32 UHDBaseAttributeSet::GetAttributeNum(UClass* AttributeSetClass) const
{
    int32 Count = 0;
    for(TFieldIterator<FProperty> Iter(AttributeSetClass); Iter; ++Iter)
    {
        const FProperty* Property = *Iter;
        if(const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
        {
            if(StructProperty->Struct == TBaseStructure<FGameplayAttributeData>::Get())
            {
                ++Count;
            }
        }
    }

    return Count;
}
