// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStepSoundCue.h"
#include "Define\HDDefine.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/GameplayStatics.h"

UAnimNotify_FootStepSoundCue::UAnimNotify_FootStepSoundCue()
    : FootSocketName(FName())
    , TraceDistance(0.f)
    , PhysicsCueTagMap{}
{
}

FString UAnimNotify_FootStepSoundCue::GetNotifyName_Implementation() const
{
    return TEXT("FootStep");
}

void UAnimNotify_FootStepSoundCue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    CONDITION_CHECK(PhysicsCueTagMap.IsEmpty());

	AActor* OwnerActor = MeshComp->GetOwner();
    NULL_CHECK(OwnerActor);

	UWorld* World = OwnerActor->GetWorld();
	VALID_CHECK(World);

    TScriptInterface<IAbilitySystemInterface> AbilitySystemInterface = OwnerActor;
    NULL_CHECK(AbilitySystemInterface);

    UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
    NULL_CHECK(ASC);

    const FVector& SocketLocation = MeshComp->GetSocketLocation(FootSocketName);
    const FVector TraceStart = SocketLocation;
    const FVector TraceEnd = TraceStart + (FVector::DownVector * TraceDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.AddIgnoredActor(OwnerActor);

    FHitResult HitResult;
    const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
    if (bHit && HitResult.PhysMaterial.IsValid())
    {
        const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
        const EPhysicsMaterialType MaterialType = GetPhysicsMaterialTypeByPhysicSurface(SurfaceType);

        CONDITION_CHECK(PhysicsCueTagMap.Contains(MaterialType) == false);

        FGameplayCueParameters Params;
        Params.Location = HitResult.ImpactPoint;
        Params.Normal = HitResult.ImpactNormal;
        Params.AggregatedSourceTags = FGameplayTagContainer(PhysicsCueTagMap[MaterialType]);

        ASC->ExecuteGameplayCue(PhysicsCueTagMap[MaterialType], Params);
    }
}

const EPhysicsMaterialType UAnimNotify_FootStepSoundCue::GetPhysicsMaterialTypeByPhysicSurface(const EPhysicalSurface PhysicSurface)
{
    EPhysicsMaterialType ret = EPhysicsMaterialType::Count;

    switch (PhysicSurface)
    {
    case SurfaceType_Default:
        ret = EPhysicsMaterialType::Default_Dirt;
    break;
    case SurfaceType1:
        ret = EPhysicsMaterialType::Stone;
    break;
    case SurfaceType2:
        ret = EPhysicsMaterialType::Wood;
    break;
    case SurfaceType3:
        ret = EPhysicsMaterialType::Metal;
    break;
    default:
        LOG("PhysicSurface is Invalid");
    break;
    }

    return ret;
}
