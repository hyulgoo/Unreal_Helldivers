// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/HDStratagemComponent.h"
#include "Define/HDDefine.h"
#include "Define/HDSocketNames.h"
#include "GameData/HDStratagemData.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Stratagem/HDStratagem.h"
#include "Abilities/GameplayAbilityTypes.h"

UHDStratagemComponent::UHDStratagemComponent()
    : StratagemClass(nullptr)
    , Stratagem(nullptr)
    , CurrentInputCommandList{}
    , SelectedStratagemName(FName())
    , SelecteddStratagemActiveDelay(0.f)
    , CommandMatchStratagemNameList{}
    , AvaliableStratagemDataTable(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UDataTable> StratagemDataListRef(TEXT("/Script/Engine.DataTable'/Game/Helldivers/GameData/DT_StratagenData.DT_StratagenData'"));
    if (StratagemDataListRef.Succeeded())
    {
        AvaliableStratagemDataTable = StratagemDataListRef.Object;
    }
}

FORCEINLINE const TArray<FName>& UHDStratagemComponent::GetCommandMatchStratagemNameList() const
{
    return CommandMatchStratagemNameList;
}

FORCEINLINE const int32 UHDStratagemComponent::GetCurrentInputNum() const
{
    return CurrentInputCommandList.Num();
}

void UHDStratagemComponent::AddStratagemCommand(const EHDCommandInput NewInput)
{
    CurrentInputCommandList.Add(NewInput);
    CommandMatchStratagemNameList.Empty();
    const TArray<FName> StratagemDataNameList = AvaliableStratagemDataTable->GetRowNames();

    const FString FindString(TEXT("Lookup"));
    for (const FName& StratagemDataName : StratagemDataNameList)
    {
        FHDStratagemData* StratagemData = AvaliableStratagemDataTable->FindRow<FHDStratagemData>(StratagemDataName, FindString);
        const TArray<EHDCommandInput>& CommandList = StratagemData->CommandSequence;
        if (CurrentInputCommandList.Num() > CommandList.Num())
        {
            continue;
        }

        bool bCommandMatch = true;
        for (int32 index = 0; index < CurrentInputCommandList.Num(); ++index)
        {
            if (CurrentInputCommandList[index] != CommandList[index])
            {
                bCommandMatch = false;
                break;
            }
        }

        if (bCommandMatch)
        {
            if (CurrentInputCommandList.Num() == CommandList.Num())
            {
                SelectedStratagemName = StratagemDataName;
                SelecteddStratagemActiveDelay = StratagemData->StratagemDelay;
            }

            CommandMatchStratagemNameList.Add(StratagemDataName);
        }
    }
}

const bool UHDStratagemComponent::IsSelectedStratagemExist() const
{
    return (SelectedStratagemName.IsNone() == false);
}

void UHDStratagemComponent::HoldStratagem(USkeletalMeshComponent* MeshComponent, const FVector& ThrowDirection)
{
    NULL_CHECK(StratagemClass);

    if (IsSelectedStratagemExist() == false)
    {
        return;
    }

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    const USkeletalMeshSocket* RightHandSocket = MeshComponent->GetSocketByName(HDSOCKETNAME_RIGRHTHAND);
    NULL_CHECK(RightHandSocket);

	const FTransform SocketTransform = RightHandSocket->GetSocketTransform(MeshComponent);
	FRotator TargetRotation = (ThrowDirection - SocketTransform.GetLocation()).Rotation();
	TargetRotation.Roll = 0;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(this);

	Stratagem = World->SpawnActor<AHDStratagem>(StratagemClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
	NULL_CHECK(Stratagem);
    
    RightHandSocket->AttachActor(Stratagem, MeshComponent);
    Stratagem->SetStratagemInfo(SelectedStratagemName, SelecteddStratagemActiveDelay);
}

void UHDStratagemComponent::ThrowFinished()
{
    VALID_CHECK(Stratagem);

    const FRotator& BaseAim = GetOwner<APawn>()->GetBaseAimRotation();
    const FVector ThrowDirection = BaseAim.Vector().GetSafeNormal();
    Stratagem->AddImpulseToStratagem(ThrowDirection); 
    Stratagem = nullptr;
}

void UHDStratagemComponent::CancelStratagem()
{
    if (Stratagem)
    {
        Stratagem->Destroy();
        Stratagem = nullptr;
    }
}

void UHDStratagemComponent::ClearCommand()
{
    SelectedStratagemName = FName();
    CommandMatchStratagemNameList.Empty();
    CurrentInputCommandList.Empty();
}
