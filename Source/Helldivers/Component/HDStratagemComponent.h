// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDStratagemComponent.generated.h"

class AHDStratagem;
class UInputAction;
enum class EHDCommandInput : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLDIVERS_API UHDStratagemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHDStratagemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UDataTable*					GetAvaliableStratagemDataTable() const;
	const TArray<FName>&	    GetCommandMatchStratagemNameList() const;
	const int32				    GetCurrentInputNum() const;
	void						AddStratagemCommand(const EHDCommandInput NewInput);
    UInputAction*               GetCommandInputAction() const;    

	const bool					IsSelectedStratagemExist() const;
	void						HoldStratagem(USkeletalMeshComponent* MeshComponent, const FVector& ThrowDirection);
	void						ThrowFinished();
	void						CancelStratagem();
	void						ClearCommand();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AHDStratagem>	StratagemClass;
    
	UPROPERTY()
	TObjectPtr<AHDStratagem>	Stratagem;
    
	UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>	CommandInputAction;

    FName						SelectedStratagemName;
    float						SelecteddStratagemActiveDelay   = 0.f;

	TArray<EHDCommandInput>		CurrentInputCommandList;
	TArray<FName>				CommandMatchStratagemNameList;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable>		AvaliableStratagemDataTable;
};
