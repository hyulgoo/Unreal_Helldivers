// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStepSoundCue.h"
#include "Define\HDDefine.h"

UAnimNotify_FootStepSoundCue::UAnimNotify_FootStepSoundCue()
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

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.AddIgnoredActor(OwnerActor);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

    if (bHit && HitResult.PhysMaterial.IsValid())
    {
        EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
    }
}
