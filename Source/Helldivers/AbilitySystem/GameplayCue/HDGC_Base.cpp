// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGC_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Define/HDDefine.h"

UHDGC_Base::UHDGC_Base()
    : bUseNiagara(false)
    , ParticleSystems{}
    , NiagaraSystems{}
    , ParticleScale(1.f)
    , SoundVolume(1.f)
    , EffectSounds{}
{
}

bool UHDGC_Base::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    if (bUseNiagara)
    {
        PlayNiagaraSystem(Target, Parameters);
    }
    else
    {
        PlayParticleSystem(Target, Parameters);
    }

    PlaySound(Target, Parameters);

    return true;
}

void UHDGC_Base::PlayNiagaraSystem(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    if (NiagaraSystems.IsEmpty() == false)
    {
        const int32 Index = FMath::RandRange(0, NiagaraSystems.Num() - 1);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, NiagaraSystems[Index], Parameters.Location, Parameters.Normal.Rotation(), FVector(ParticleScale));
    }
}

void UHDGC_Base::PlayParticleSystem(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    if (ParticleSystems.IsEmpty() == false)
    {
        const int32 Index = FMath::RandRange(0, ParticleSystems.Num() - 1);
        UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystems[Index], Parameters.Location, Parameters.Normal.Rotation(), FVector(ParticleScale));
    }
}

void UHDGC_Base::PlaySound(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    if (EffectSounds.IsEmpty() == false)
    {
        const int32 Index = FMath::RandRange(0, EffectSounds.Num() - 1);
        UGameplayStatics::PlaySoundAtLocation(Target, EffectSounds[Index], Parameters.Location, SoundVolume);
    }
}
