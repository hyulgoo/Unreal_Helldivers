// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/Cue/HDGC_BulletHit.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

UHDGC_BulletHit::UHDGC_BulletHit()
{
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionRef(TEXT(""));
    //if(ExplosionRef.Object)
    //{
    //    ParticleSystem = ExplosionRef.Object;
    //}
}

bool UHDGC_BulletHit::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
    if(HitResult)
    {
        UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, HitResult->ImpactPoint, FRotator::ZeroRotator, true);
    }
    else
    {
        for(const auto& TargetActor : Parameters.EffectContext.Get()->GetActors())
        {
            if(IsValid(TargetActor.Get()))
            {
                UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, TargetActor->GetActorLocation(), FRotator::ZeroRotator, true);
            }
        }
    }

    return false;
}
