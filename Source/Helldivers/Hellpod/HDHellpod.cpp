// Fill out your copyright notice in the Description page of Project Settings.

#include "Hellpod/HDHellpod.h"
#include "Define/HDDefine.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/HDGASPlayerState.h"
#include "Character/HDCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Collision/HDCollision.h"
#include "Controller/HDPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AHDHellpod::AHDHellpod()
	: InputForward(0.f)
	, InputRight(0.f)
	, CurrentInput{}
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetNotifyRigidBodyCollision(true);
	CollisionBox->SetEnableGravity(true);
	CollisionBox->SetCollisionProfileName(COLLISION_PROFILE_HDHELLPOD);
	CollisionBox->BodyInstance.bUseCCD = true;
	CollisionBox->OnComponentHit.AddDynamic(this, &AHDHellpod::OnBoxHit);

	HellpodMesh = CreateDefaultSubobject<UStaticMeshComponent>("HellpodMesh");
	HellpodMesh->SetupAttachment(RootComponent);
	HellpodMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AHDHellpod::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat SpawnCharacterTimelineProgress;
	SpawnCharacterTimelineProgress.BindUFunction(this, FName("OnSpawnTimelineUpdate"));
	SpawnCharacterTimeline.AddInterpFloat(SpawnCurveFloat, SpawnCharacterTimelineProgress);
	FOnTimelineEventStatic SpawnCharacterTimelineEventStatic;
	SpawnCharacterTimelineEventStatic.BindUObject(this, &AHDHellpod::SpawnCharacterEnd);
	SpawnCharacterTimeline.SetTimelineFinishedFunc(SpawnCharacterTimelineEventStatic);
	SpawnCharacterTimeline.SetLooping(false);

	NULL_CHECK(HellpodMesh);
	MeshDefaultRelativeRotation = HellpodMesh->GetRelativeRotation();

	NULL_CHECK(CollisionBox);
	CollisionBox->AddImpulse(FVector::DownVector * FallSpeed, NAME_None, true);
}

void AHDHellpod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnCharacterTimeline.TickTimeline(DeltaTime);	
	RotateHellpodByCurrentImpulse(DeltaTime);
}

void AHDHellpod::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	NULL_CHECK(PlayerController);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	NULL_CHECK(Subsystem);

	Subsystem->ClearAllMappings();
	NULL_CHECK(InputMappingContext);

	Subsystem->AddMappingContext(InputMappingContext, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	NULL_CHECK(EnhancedInputComponent);
	NULL_CHECK(MoveAction);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHDHellpod::MoveHellpod);
}

void AHDHellpod::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CONDITION_CHECK(ImpactTag.IsValid() == false);
	NULL_CHECK(OtherActor);

	HellpodMesh->SetRelativeRotation(FRotator(0.f, 0.f, 180.f));

	CollisionBox->SetSimulatePhysics(false);
	CollisionBox->SetEnableGravity(false);

	AHDGASPlayerState* GASPlayerState = GetPlayerState<AHDGASPlayerState>();
	NULL_CHECK(GASPlayerState);

	UAbilitySystemComponent* PlayerStateASC = GASPlayerState->GetAbilitySystemComponent();
	NULL_CHECK(PlayerStateASC);

	SpawnCharacter();
	SpawnCharacterTimeline.PlayFromStart();

	FGameplayCueParameters Params;
	Params.Location = Hit.ImpactPoint;
	Params.Normal = Hit.Normal;
	Params.SourceObject = this;
	PlayerStateASC->ExecuteGameplayCue(ImpactTag, Params);
}

void AHDHellpod::SpawnCharacter()
{
	NULL_CHECK(CharacterClass);

	UWorld* World = GetWorld();
	VALID_CHECK(World);

	const FTransform& CurrentHellpodTransform = GetActorTransform();
	SpawnedCharacter = World->SpawnActorDeferred<AHDCharacterPlayer>(
		CharacterClass,
		CurrentHellpodTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	NULL_CHECK(SpawnedCharacter);

	APlayerController* PlayerController = GetController<AHDPlayerController>();
	NULL_CHECK(PlayerController);

	PlayerController->Possess(Cast<APawn>(SpawnedCharacter));
	PlayerController->SetViewTargetWithBlend(Cast<APawn>(SpawnedCharacter), 1.f, EViewTargetBlendFunction::VTBlend_Cubic);
	SpawnedCharacter->GetCapsuleComponent()->Activate(false);

	UGameplayStatics::FinishSpawningActor(SpawnedCharacter, CurrentHellpodTransform);
	PlayerController = SpawnedCharacter->GetController<APlayerController>();
	NULL_CHECK(PlayerController);
	SpawnedCharacter->DisableInput(PlayerController);
}

void AHDHellpod::OnSpawnTimelineUpdate(const float Value)
{
	NULL_CHECK(SpawnedCharacter);

	FVector CharacterOrgin, CharacterExtent;
	SpawnedCharacter->GetActorBounds(true, CharacterOrgin, CharacterExtent);

	const FVector HellpodLocation = GetActorLocation();
	const float Interpolated			= FMath::Lerp(HellpodLocation.Z, CharacterExtent.Z, Value);
	const FVector& CharacterLocation	= SpawnedCharacter->GetActorLocation();
	const FVector NewLocation(CharacterLocation.X, CharacterLocation.Y, Interpolated);
	SpawnedCharacter->SetActorLocation(NewLocation);
}

void AHDHellpod::SpawnCharacterEnd()
{
	APlayerController* PlayerController = SpawnedCharacter->GetController<APlayerController>();
	NULL_CHECK(PlayerController);
	SpawnedCharacter->EnableInput(PlayerController);

	NULL_CHECK(SpawnedCharacter);
	SpawnedCharacter->GetCapsuleComponent()->Activate(true);

	SetLifeSpan(30.f);
}

void AHDHellpod::MoveHellpod(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	CurrentInput = MovementVector;

	const FRotator& Rotation = Controller->GetControlRotation();

	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	CollisionBox->AddImpulse(FVector::RightVector * MovementVector.Y * MaxMoveSpeed);
	CollisionBox->AddImpulse(FVector::ForwardVector * MovementVector.X * MaxMoveSpeed);
}

void AHDHellpod::RotateHellpodByCurrentImpulse(const float DeltaTime)
{
	const float TargetPitch = CurrentInput.X * MaxPitchAngle;
	const float TargetRoll = -CurrentInput.Y * MaxRollAngle;

	const FRotator& CurrentRotator = HellpodMesh->GetRelativeRotation();
	const FRotator TargetRotation(
		MeshDefaultRelativeRotation.Pitch + TargetPitch,
		MeshDefaultRelativeRotation.Yaw,
		MeshDefaultRelativeRotation.Roll + TargetRoll);

	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotator,
		TargetRotation,
		DeltaTime,
		5.f
	);

	UE_LOG(LogTemp, Error, TEXT("NewRotation is [%s]"), *NewRotation.ToString());

	HellpodMesh->SetRelativeRotation(NewRotation);
}
