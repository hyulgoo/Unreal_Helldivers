
#include "HDCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterTypes/HDCharacterStateTypes.h"
#include "HDCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "Collision/HDCollision.h"
#include "Define/HDDefine.h"

AHDCharacterBase::AHDCharacterBase()
	: FireWeaponMontage(nullptr)
    , ThrowMontage(nullptr)
    , DeadMontage(nullptr)
    , DeadEventDelayTime(5.f)
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw	= false;
	bUseControllerRotationRoll	= false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(HDCOLLISION_PROFILE_CAPSULE);

	// Movement
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement	= true;
	CharacterMovementComponent->RotationRate				= FRotator(0.f, 45.f, 0.f);
	CharacterMovementComponent->JumpZVelocity				= 700.f;
	CharacterMovementComponent->AirControl					= 0.35f;
	CharacterMovementComponent->MaxWalkSpeed				= 500.f;
	CharacterMovementComponent->MinAnalogWalkSpeed			= 20.f;
	CharacterMovementComponent->BrakingDecelerationWalking	= 2000.f;

	// Mesh
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SkeletalMeshComponent->SetCollisionProfileName(HDCOLLISION_PROFILE_PLAYER);

	LoadDefaultMontage();
}

void AHDCharacterBase::SetDead()
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->SetSimulatePhysics(true);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHDCharacterBase::PlayDeadAnimation()
{
	NULL_CHECK(DeadMontage);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	NULL_CHECK(AnimInstance);

	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeadMontage, 1.f);
}

void AHDCharacterBase::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
	// Ragdoll로 만들어야 하는데 AddImpulse가 없는 경우 Error
	CONDITION_CHECK(bRagdoll && Impulse == FVector::ZeroVector);

	USkeletalMeshComponent* CharacterMesh = GetMesh();
	CharacterMesh->SetSimulatePhysics(bRagdoll);

	if (bRagdoll)
	{
		GetCharacterMovement()->DisableMovement();
		CharacterMesh->AddImpulse(Impulse, NAME_None, true);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		const FVector& BonmeLocation = CharacterMesh->GetBoneLocation("pelvis");
		const FRotator NewMeshRotation(0.f, CharacterMesh->GetComponentRotation().Yaw, 0.f);

		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldLocation(BonmeLocation);
		Capsule->SetWorldRotation(NewMeshRotation);

		//CharacterMesh->AttachToComponent(Capsule, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CharacterMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
		CharacterMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
}

const float AHDCharacterBase::GetRagdollPysicsLinearVelocity() const
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	return CharacterMesh->GetPhysicsLinearVelocity().Size();
}

void AHDCharacterBase::LoadDefaultMontage()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
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
