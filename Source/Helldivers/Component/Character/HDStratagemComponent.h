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
	explicit UHDStratagemComponent();

	UDataTable*					GetAvaliableStratagemDataTable() { return AvaliableStratagemDataTable; }

	FORCEINLINE const TArray<FName>& GetCommandMatchStratagemNameList() const;
	FORCEINLINE const int32		GetCurrentInputNum() const;
	void						AddStratagemCommand(const EHDCommandInput NewInput);

	const bool					IsSelectedStratagemExist() const;
	void						HoldStratagem(USkeletalMeshComponent* MeshComponent, const FVector& ThrowDirection);
	void						ThrowFinished();
	void						CancleStratagem();
	void						ClearCommand();

private:
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TSubclassOf<AHDStratagem>	StratagemClass;

	UPROPERTY()
	TObjectPtr<AHDStratagem>	Stratagem;
	
	UPROPERTY()
	TArray<EHDCommandInput>		CurrentInputCommandList;

	FName						SelectedStratagemName;
	float						SelecteddStratagemActiveDelay;

	UPROPERTY()
	TArray<FName>				CommandMatchStratagemNameList;
	
	UPROPERTY(EditAnywhere, Category = "Stratagem")
	TObjectPtr<UDataTable>		AvaliableStratagemDataTable;
};
