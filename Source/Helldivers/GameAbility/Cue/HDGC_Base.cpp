// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/Cue/HDGC_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

UHDGC_Base::UHDGC_Base()
{    
}

bool UHDGC_Base::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
    if(HitResult)
    {
        if (ParticleSystem)
        {
            UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, HitResult->ImpactPoint, FRotator::ZeroRotator, true);
        }

        if (EffectSound)
        {
            UGameplayStatics::PlaySoundAtLocation(Target, EffectSound, HitResult->ImpactPoint);
        }
    }
    else
    {
        for(const auto& TargetActor : Parameters.EffectContext.Get()->GetActors())
        {
            if(IsValid(TargetActor.Get()))
            {
                FVector TargetLocation = TargetActor->GetActorLocation();
                if (ParticleSystem)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, TargetActor->GetActorLocation(), FRotator::ZeroRotator, true);
                }

                if(EffectSound)
                {
                    UGameplayStatics::PlaySoundAtLocation(Target, EffectSound, TargetLocation);
                }
            }
        }
    }

    return false;
}
