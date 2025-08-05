// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDStratagemComponent.generated.h"

class AHDStratagem;
enum class EHDCommandInput : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLDIVERS_API UHDStratagemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHDStratagemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UDataTable*							GetAvaliableStratagemDataTable() const;
	FORCEINLINE const TArray<FName>&	GetCommandMatchStratagemNameList() const;
	FORCEINLINE const int32				GetCurrentInputNum() const;
	void								AddStratagemCommand(const EHDCommandInput NewInput);

	const bool							IsSelectedStratagemExist() const;
	void								HoldStratagem(USkeletalMeshComponent* MeshComponent, const FVector& ThrowDirection);
	void								ThrowFinished();
	void								CancelStratagem();
	void								ClearCommand();

private:
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TSubclassOf<AHDStratagem>			StratagemClass;
        
	UPROPERTY()
	TObjectPtr<AHDStratagem>			Stratagem;

    FName								SelectedStratagemName           = FName();
    float								SelecteddStratagemActiveDelay   = 0.f;

	TArray<EHDCommandInput>				CurrentInputCommandList;
	TArray<FName>						CommandMatchStratagemNameList;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TObjectPtr<UDataTable>				AvaliableStratagemDataTable;
};
