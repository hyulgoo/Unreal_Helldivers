#include "HDProjectileBase.h"  
#include "Components/BoxComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "NiagaraComponent.h"  
#include "Kismet/GameplayStatics.h"  
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameAbility/Effect/HDGE_ApplyDamage.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Collision/HDCollision.h"

AHDProjectileBase::AHDProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionProfileName(COLLISION_PROFILE_HDPROJECTILE);
	CollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);
	CollisionBox->OnComponentHit.AddDynamic(this, &AHDProjectileBase::OnBoxHit);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AHDProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeBeginPlay();
	StartDestroyTimer();
	SpawnTrailSystem();
}

void AHDProjectileBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AHDProjectileBase::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("DropTargetPosition : %s"), *GetActorLocation().ToString());

	Super::Destroyed();
}

void AHDProjectileBase::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CONDITION_CHECK(ImpactBlocklCueTag.IsValid() == false || ImpactHitCueTag.IsValid() == false);
	NULL_CHECK(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("ProjectileHitActor : %s"), *OtherActor->GetName());

	AActor* OwnerActor = GetOwner();
	NULL_CHECK(OwnerActor);

	IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerActor);
	NULL_CHECK(OwnerAbilitySystemInterface);

	UAbilitySystemComponent* OwnerAbilitySystemComponent = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
	NULL_CHECK(OwnerAbilitySystemComponent);

	// Get Target ASC
	IAbilitySystemInterface* TargetAbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor);
	if (TargetAbilitySystemInterface)
	{
		UAbilitySystemComponent* TargetAbilitySystemComponent = TargetAbilitySystemInterface->GetAbilitySystemComponent();
		NULL_CHECK(TargetAbilitySystemComponent);

		ApplyImpactGameEffect(TargetAbilitySystemComponent);
		ApplyImpactGameAbility(TargetAbilitySystemComponent);
		ExcuteGameplayCue(OwnerAbilitySystemComponent, ImpactHitCueTag, Hit);
	}
	else
	{
		ExcuteGameplayCue(OwnerAbilitySystemComponent, ImpactBlocklCueTag, Hit);
	}

	Destroy();
}

void AHDProjectileBase::InitializeBeginPlay()
{
	CollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);

	ProjectileMovementComponent->MaxSpeed = InitSpeed;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AHDProjectileBase::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AHDProjectileBase::DestroyTimerFinished, DestroyTime, false);
}

void AHDProjectileBase::DestroyTimerFinished()
{
	Destroy();
}

void AHDProjectileBase::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, RootComponent, FName(),
			GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
		return;
	}

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			GetActorScale(),
			EAttachLocation::KeepWorldPosition
		);
	}
}

void AHDProjectileBase::ApplyImpactGameEffect(UAbilitySystemComponent* TargetAbiltySystemComponent)
{
	NULL_CHECK(ImpactGameEffect);
	VALID_CHECK(TargetAbiltySystemComponent);

	const FGameplayEffectContextHandle Context = TargetAbiltySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle ImpactGameEffectSpecHandle = TargetAbiltySystemComponent->MakeOutgoingSpec(ImpactGameEffect, 1.f, Context);
	CONDITION_CHECK(ImpactGameEffectSpecHandle.IsValid() == false);

	ImpactGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DAMAGE_PROJECTILE, -ImpactDamage);
	TargetAbiltySystemComponent->ApplyGameplayEffectSpecToSelf(*ImpactGameEffectSpecHandle.Data.Get());

	if (StatusEffect != EStatusEffect::None)
	{
		NULL_CHECK(StatusGameEffect);

		const FGameplayEffectSpecHandle StatusGameEffectSpecHandle = TargetAbiltySystemComponent->MakeOutgoingSpec(StatusGameEffect, 1.f, Context);
		CONDITION_CHECK(StatusGameEffectSpecHandle.IsValid() == false);

		StatusGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DOTDAMAGE_TICKDAMAGE, -DotDamage);
		StatusGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DOTDAMAGE_DURATION, StatusDuration);
		TargetAbiltySystemComponent->ApplyGameplayEffectSpecToSelf(*StatusGameEffectSpecHandle.Data.Get());
	}
}

void AHDProjectileBase::ApplyImpactGameAbility(UAbilitySystemComponent* TargetAbiltySystemComponent)
{
	NULL_CHECK(TargetAbiltySystemComponent);

	CONDITION_CHECK(KnockbackTag.IsValid() == false);

	FGameplayEventData EventData;
	EventData.EventTag = KnockbackTag;
	EventData.Instigator = this;
	EventData.Target = TargetAbiltySystemComponent->GetOwnerActor();
	EventData.EventMagnitude = 300.f;

	TargetAbiltySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);

	UE_LOG(LogTemp, Warning, TEXT("ApplyImpactGameAbility Called!"));
}

void AHDProjectileBase::ExcuteGameplayCue(UAbilitySystemComponent* OwnerAbilitySystemComponent, const FGameplayTag& Tag, const FHitResult& Hit)
{
	NULL_CHECK(OwnerAbilitySystemComponent);
	CONDITION_CHECK(Tag.IsValid() == false);

	FGameplayCueParameters Params;
	Params.Location = Hit.ImpactPoint;
	Params.Normal = Hit.Normal;
	Params.Instigator = OwnerAbilitySystemComponent->GetOwnerActor();
	Params.SourceObject = this;
	OwnerAbilitySystemComponent->ExecuteGameplayCue(Tag, Params);
}
