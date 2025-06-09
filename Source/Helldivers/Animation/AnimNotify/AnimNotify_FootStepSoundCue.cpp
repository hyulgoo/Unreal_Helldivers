// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStepSoundCue.h"
#include "Define\HDDefine.h"
#include "Kismet/GameplayStatics.h"

UAnimNotify_FootStepSoundCue::UAnimNotify_FootStepSoundCue()
    : FootSocketName(FName())
    , TraceDistance(0.f)
    , PhysicsSoundMap{}
{
}

FString UAnimNotify_FootStepSoundCue::GetNotifyName_Implementation() const
{
    return TEXT("FootStep");
}

void UAnimNotify_FootStepSoundCue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    CONDITION_CHECK(PhysicsSoundMap.IsEmpty());

    AActor* OwnerActor = MeshComp->GetOwner();
    NULL_CHECK(OwnerActor);

    const FVector& SocketLocation = MeshComp->GetSocketLocation(FootSocketName);
    const FVector TraceStart = SocketLocation;
    const FVector TraceEnd = TraceStart + (FVector::DownVector * TraceDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.AddIgnoredActor(OwnerActor);

	UWorld* World = OwnerActor->GetWorld();
	VALID_CHECK(World);

    FHitResult HitResult;
    const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
    if (bHit && HitResult.PhysMaterial.IsValid())
    {
        const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
        const EPhysicsMaterialType MaterialType = GetPhysicsMaterialTypeByPhysicSurface(SurfaceType);

        CONDITION_CHECK(PhysicsSoundMap.Contains(MaterialType) == false);

        UGameplayStatics::PlaySoundAtLocation(World, PhysicsSoundMap[MaterialType], SocketLocation);
    }
}

const EPhysicsMaterialType UAnimNotify_FootStepSoundCue::GetPhysicsMaterialTypeByPhysicSurface(const EPhysicalSurface PhysicSurface)
{
    EPhysicsMaterialType ret = EPhysicsMaterialType::Count;

    switch (PhysicSurface)
    {
    case SurfaceType_Default:
        ret = EPhysicsMaterialType::Default;
    break;
    case SurfaceType1:
        ret = EPhysicsMaterialType::Dirt;
    break;
    case SurfaceType2:
        ret = EPhysicsMaterialType::Wood;
    break;
    case SurfaceType3:
        ret = EPhysicsMaterialType::Metal;
    break;
    default:
        CONDITION_CHECK_WITH_RETURNTYPE(ret == EPhysicsMaterialType::Count, ret);
    break;
    }

    return ret;
}
