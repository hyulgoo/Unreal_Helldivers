#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HDCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FHDCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FHDCharacterStat()
		: MaxHealth(0.f)
		, CrawlingSpeed(0.f)
		, CrouchSpeed(0.f)
		, WalkSpeed(0.f)
		, SprintSpeed(0.f)
		, MaxStamina(0.f)
	{
	};

	FHDCharacterStat operator+(const FHDCharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FHDCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FHDCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}

public:
	// Base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHealth;

	// Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CrawlingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxStamina;
};
