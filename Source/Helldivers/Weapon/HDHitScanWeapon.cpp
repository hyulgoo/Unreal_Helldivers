// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/HDHitScanWeapon.h"
#include "Character/HDCharacterPlayer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameAbility/Effect/HDGE_ApplyDamage.h"
#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"

AHDHitScanWeapon::AHDHitScanWeapon()
{
    FireType = EFireType::HitScan;

    static ConstructorHelpers::FClassFinder<UHDGE_ApplyDamage> FireDamageEffectRef(TEXT("/Game/Helldivers/Blueprint/GameAbility/Effect/BP_GE_FireDamage.BP_GE_FireDamage_C"));
    if (FireDamageEffectRef.Class)
    {
        FireDamageEffect = FireDamageEffectRef.Class;
    }
}

void AHDHitScanWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr)
    {
        return;
    }

    AController* InstigatorController = OwnerPawn->GetController();
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        const FVector Start = SocketTransform.GetLocation();

        FHitResult FireHit;
        WeaponTraceHit(Start, HitTarget, FireHit);

        AHDCharacterPlayer* BlasterCharacter = Cast<AHDCharacterPlayer>(FireHit.GetActor());
        if (BlasterCharacter && InstigatorController)
        {
            const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? Damage * HeadShotDamageRate : Damage;
            UGameplayStatics::ApplyDamage(BlasterCharacter, DamageToCause, InstigatorController, this, UDamageType::StaticClass());
        }

        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
        }

        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
        }

        if (MuzzleFlash)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
        }

        if (FireSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
        }
    }
}

void AHDHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return;
    }

    const FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
    const bool bHit = World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECollisionChannel::ECC_Visibility);

    FVector BeamEnd = End;
    if (OutHit.bBlockingHit)
    {
        BeamEnd = OutHit.ImpactPoint;
    }
    else
    {
        OutHit.ImpactPoint = End;
    }

    if (BeamParticles)
    {
        UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, TraceStart, FRotator::ZeroRotator, true);
        if (Beam)
        {
            Beam->SetVectorParameter(FName("Target"), BeamEnd);
        }
    }

    if (bHit)
    {
        AHDGASCharacterPlayer* TargetGASPlayer = Cast<AHDGASCharacterPlayer>(OutHit.GetActor());
        if (TargetGASPlayer)
        {
            UAbilitySystemComponent* TargetASC = TargetGASPlayer->GetAbilitySystemComponent();
            if (TargetASC && FireDamageEffect)
            {
                FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(FireDamageEffect, 1.f, TargetASC->MakeEffectContext());
                if (EffectSpecHandle.IsValid())
                {
                    FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
                    if (EffectSpec)
                    {
                        EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Damage")), Damage);
                        TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
                    }
                }
            }
        }
    }
}
