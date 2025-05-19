
#include "HDCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HDCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "Collision/HDCollision.h"
#include "CharacterTypes/HDCharacterStateTypes.h"

AHDCharacterBase::AHDCharacterBase()
	: DeadEventDelayTime(5.f)
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw	= false;
	bUseControllerRotationRoll	= false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(COLLISION_PROFILE_HDPLAYER);

	// Movement
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement	= true;
	CharacterMovementComponent->RotationRate				= FRotator(0.f, 500.f, 0.f);
	CharacterMovementComponent->JumpZVelocity				= 700.f;
	CharacterMovementComponent->AirControl					= 0.35f;
	CharacterMovementComponent->MaxWalkSpeed				= 500.f;
	CharacterMovementComponent->MinAnalogWalkSpeed			= 20.f;
	CharacterMovementComponent->BrakingDecelerationWalking	= 2000.f;

	// Mesh
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (CharacterMeshRef.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(CharacterMeshRef.Object);
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Helldivers/Animation/ABP_HDCharacter.ABP_HDCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		SkeletalMeshComponent->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UHDCharacterControlData> ThirdPersonDataRef(TEXT("/Script/Helldivers.HDCharacterControlData'/Game/Helldivers/CharacterControl/HDC_ThirdPerson.HDC_ThirdPerson'"));
	if (ThirdPersonDataRef.Succeeded())
	{
		CharacterControlDataMap.Add(EHDCharacterControlType::ThirdPerson, ThirdPersonDataRef.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UHDCharacterControlData> FirstPersonDataRef(TEXT("/Script/Helldivers.HDCharacterControlData'/Game/Helldivers/CharacterControl/HDC_FirstPerson.HDC_FirstPerson'"));
	if (FirstPersonDataRef.Succeeded())
	{
		CharacterControlDataMap.Add(EHDCharacterControlType::FirstPerson, FirstPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireWeaponMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Helldivers/Animation/AM_Fire.AM_Fire'"));
	if (FireWeaponMontageRef.Succeeded())
	{
		FireWeaponMontage = FireWeaponMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ThrowMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Helldivers/Animation/AM_Throw.AM_Throw'"));
	if (ThrowMontageRef.Succeeded())
	{
		ThrowMontage = ThrowMontageRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Helldivers/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object; 
	}
}

void AHDCharacterBase::SetCharacterControlData(UHDCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
		CharacterMovementComponent->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
		CharacterMovementComponent->RotationRate = CharacterControlData->RotationRate;
	}
}

void AHDCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void AHDCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.f);
		AnimInstance->Montage_Play(DeadMontage, 1.f);
	}
}
