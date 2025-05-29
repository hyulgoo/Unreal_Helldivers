// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGC_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Define/HDDefine.h"

UHDGC_Base::UHDGC_Base()
    : ParticleSystem(nullptr)
    , ParticleScale(0.f)
    , EffectSounds{}
{
    ParticleScale = 1.f;
}

bool UHDGC_Base::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    if (ParticleSystem)
    {   
        UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, Parameters.Location, Parameters.Normal.Rotation(), FVector(ParticleScale));
    }

    if (EffectSounds.IsEmpty() == false)
    {
        const int32 Index = FMath::RandRange(0, EffectSounds.Num() - 1);
        UGameplayStatics::PlaySoundAtLocation(Target, EffectSounds[Index], Parameters.Location);
    }

    return true;
}
