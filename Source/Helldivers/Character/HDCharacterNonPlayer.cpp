// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/HDCharacterNonPlayer.h"
#include "Engine/AssetManager.h"

AHDCharacterNonPlayer::AHDCharacterNonPlayer()
{
    GetMesh()->SetHiddenInGame(true);

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AHDCharacterNonPlayer::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    ensure(NPCMeshes.Num() > 0);
    const int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
    NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &AHDCharacterNonPlayer::NPCMeshLoadCompleted));
}

void AHDCharacterNonPlayer::SetDead()
{
    Super::SetDead();

    FTimerHandle DeadTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
        [&]() 
        { 
            Destroy(); 
        }
    ), DeadEventDelayTime, false);
}

void AHDCharacterNonPlayer::NPCMeshLoadCompleted()
{
    if (NPCMeshHandle.IsValid())
    {
        USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
        if (NPCMesh)
        {
            GetMesh()->SetSkeletalMesh(NPCMesh);
            GetMesh()->SetHiddenInGame(false);
        }
    }

    NPCMeshHandle->ReleaseHandle();
}
