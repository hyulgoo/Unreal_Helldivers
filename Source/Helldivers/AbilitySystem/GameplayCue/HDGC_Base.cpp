// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGC_Base.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Define/HDDefine.h"

bool UHDGC_Base::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    NULL_CHECK_WITH_RETURNTYPE(Target, false);

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
    VALID_CHECK(Target);
    CONDITION_CHECK_WITHOUT_LOG(NiagaraSystems.IsEmpty() == false);

    const int32 Index = FMath::RandRange(0, NiagaraSystems.Num() - 1);
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, NiagaraSystems[Index], Parameters.Location, Parameters.Normal.Rotation(), FVector(ParticleScale));
}

void UHDGC_Base::PlayParticleSystem(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    VALID_CHECK(Target);
    CONDITION_CHECK_WITHOUT_LOG(ParticleSystems.IsEmpty() == false);

    const int32 Index = FMath::RandRange(0, ParticleSystems.Num() - 1);
    UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystems[Index], Parameters.Location, Parameters.Normal.Rotation(), FVector(ParticleScale));
}

void UHDGC_Base::PlaySound(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    VALID_CHECK(Target);
    CONDITION_CHECK_WITHOUT_LOG(EffectSounds.IsEmpty() == false);

    const int32 Index = FMath::RandRange(0, EffectSounds.Num() - 1);
    UGameplayStatics::PlaySoundAtLocation(Target, EffectSounds[Index], Parameters.Location, SoundVolume);
}
