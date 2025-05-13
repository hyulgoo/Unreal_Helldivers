// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/Cue/HDGC_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Define/HDDefine.h"

bool UHDGC_Base::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    if (ParticleSystem)
    {
        UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, Parameters.Location, Parameters.Normal.Rotation(), true);
    }

    if (EffectSounds.IsEmpty() == false)
    {
        const int32 Index = FMath::RandRange(0, EffectSounds.Num() - 1);
        UGameplayStatics::PlaySoundAtLocation(Target, EffectSounds[Index], Parameters.Location);
    }

    return true;
}
