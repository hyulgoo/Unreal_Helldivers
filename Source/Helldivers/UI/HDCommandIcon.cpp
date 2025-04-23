// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HDCommandIcon.h"
#include "Define/HDDefine.h"
#include "Components/Image.h"

void UHDCommandIcon::SetCommandIcon(UTexture2D* IconTexture)
{
    NULL_CHECK(Img_CommandIcon);
    NULL_CHECK(IconTexture);

    Img_CommandIcon->SetBrushFromTexture(IconTexture);
}
