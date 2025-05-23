// Fill out your copyright notice in the Description page of Project Settings.

#include "Hellpod/HDHellpod.h"
#include "Define/HDDefine.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/HDGASPlayerState.h"
#include "Character/HDCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Collision/HDCollision.h"
#include "Controller/HDPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AHDHellpod::AHDHellpod()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionProfileName(COLLISION_PROFILE_HDHELLPOD);
	CollisionBox->OnComponentHit.AddDynamic(this, &AHDHellpod::OnBoxHit);

	HellpodMesh = CreateDefaultSubobject<UStaticMeshComponent>("HellpodMesh");
	HellpodMesh->SetupAttachment(RootComponent);
	HellpodMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionBox);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

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

	ProjectileMovementComponent->MaxSpeed = FallSpeed;
	ProjectileMovementComponent->Velocity = FVector::DownVector * FallSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	FOnTimelineFloat SpawnCharacterTimelineProgress;
	SpawnCharacterTimelineProgress.BindUFunction(this, FName("OnSpawnTimelineUpdate"));
	SpawnCharacterTimeline.AddInterpFloat(SpawnCurveFloat, SpawnCharacterTimelineProgress);
	FOnTimelineEventStatic SpawnCharacterTimelineEventStatic;
	SpawnCharacterTimelineEventStatic.BindUObject(this, &AHDHellpod::SpawnCharacterEnd);
	SpawnCharacterTimeline.SetTimelineFinishedFunc(SpawnCharacterTimelineEventStatic);
	SpawnCharacterTimeline.SetLooping(false);
}

void AHDHellpod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnCharacterTimeline.TickTimeline(DeltaTime);	
}

void AHDHellpod::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHDHellpod::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Error, TEXT("OnBoxHit Called!!!!!"));

	NULL_CHECK(ProjectileMovementComponent);
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;

	CONDITION_CHECK(ImpactTag.IsValid() == false);
	NULL_CHECK(OtherActor);

	AHDGASPlayerState* GASPlayerState = GetPlayerState<AHDGASPlayerState>();
	NULL_CHECK(GASPlayerState);

	UAbilitySystemComponent* PlayerStateASC = GASPlayerState->GetAbilitySystemComponent();
	NULL_CHECK(PlayerStateASC);

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
	SpawnedCharacter = World->SpawnActorDeferred<AHDCharacterPlayer>(CharacterClass, CurrentHellpodTransform);
	NULL_CHECK(SpawnedCharacter);

	SpawnedCharacter->GetCapsuleComponent()->Activate(false);

	UGameplayStatics::FinishSpawningActor(SpawnedCharacter, CurrentHellpodTransform);

	AController* PlayerController = GetController();
	NULL_CHECK(PlayerController);
	PlayerController->Possess(Cast<APawn>(SpawnedCharacter));
}

void AHDHellpod::OnSpawnTimelineUpdate(const float Value)
{
	NULL_CHECK(SpawnedCharacter);

	const float ActorLocationZ			= GetActorLocation().Z;
	const float TargetLocationZ			= GetActorLocation().Z + GetActorScale().Z;
	const float Interpolated			= FMath::Lerp(ActorLocationZ, TargetLocationZ, Value);
	const FVector& CharacterLocation	= SpawnedCharacter->GetActorLocation();
	const FVector NewLocation(CharacterLocation.X, CharacterLocation.Y, Interpolated);
	SpawnedCharacter->SetActorLocation(NewLocation);
}

void AHDHellpod::SpawnCharacterEnd()
{
	NULL_CHECK(SpawnedCharacter);
	SpawnedCharacter->GetCapsuleComponent()->Activate(true);

	AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
	NULL_CHECK(PlayerController);
	SpawnedCharacter->DisableInput(PlayerController);
}
