#include "HDProjectileBase.h"  
#include "Components/BoxComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "NiagaraComponent.h"  
#include "Kismet/GameplayStatics.h"  
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/GameplayEffect/HDGE_ApplyDamage.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Collision/HDCollision.h"
#include "Engine/OverlapResult.h"

AHDProjectileBase::AHDProjectileBase()
	: ProjectileMesh(nullptr)
	, ProjectileMovementComponent(nullptr)
	, CollisionBox(nullptr)
	, ProjectileTag(FGameplayTag())
	, InitSpeed(0.f)
	, ImpactType(EImpactType::Hit)
	, ImpactGameEffect(nullptr)
	, ImpactDamage(0.f)
	, ImpactBlocklCueTag(FGameplayTag())
	, ImpactHitCueTag(FGameplayTag())
	, StatusEffect(EStatusEffect::None)
	, StatusGameEffect(nullptr)
	, DotDamage(0.f)
	, StatusDuration(0.f)
	, ExplodeDamageRange(0.f)
	, ExplodeKnockBackRange(0.f)
	, KnockbackTag(FGameplayTag())
	, KnockbackImpulse(0.f)
	, Tracer(nullptr)
	, TracerComponent(nullptr)
	, TrailSystem(nullptr)
	, TrailSystemComponent(nullptr)
	, DestroyTime (10.f)
	, DestroyTimer(FTimerHandle())
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionProfileName(HDCOLLISION_PROFILE_PROJECTILE);
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
	Super::Destroyed();
}

void AHDProjectileBase::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	NULL_CHECK(OtherActor);

	AActor* OwnerActor = GetOwner();
	NULL_CHECK(OwnerActor);

	IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerActor);
	NULL_CHECK(OwnerAbilitySystemInterface);

	UAbilitySystemComponent* OwnerAbilitySystemComponent = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
	NULL_CHECK(OwnerAbilitySystemComponent);

	if (ImpactType == EImpactType::Explode)
	{
		ApplyExplode(Hit.ImpactPoint);
		ExcuteGameplayCue(OwnerAbilitySystemComponent, ImpactHitCueTag, Hit);
	}
	else
	{
		// Get Target ASC
		IAbilitySystemInterface* TargetAbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor);
		if (TargetAbilitySystemInterface)
		{
			UAbilitySystemComponent* TargetAbilitySystemComponent = TargetAbilitySystemInterface->GetAbilitySystemComponent();
			NULL_CHECK(TargetAbilitySystemComponent);

			ApplyImpactGameEffect(TargetAbilitySystemComponent, ImpactDamage);
			ExcuteGameplayCue(OwnerAbilitySystemComponent, ImpactHitCueTag, Hit);
		}
		else
		{
			ExcuteGameplayCue(OwnerAbilitySystemComponent, ImpactBlocklCueTag, Hit);
		}
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

void AHDProjectileBase::ApplyExplode(const FVector& HitLocation)
{
	UWorld* World = GetWorld();
	VALID_CHECK(World);

	TArray<FHitResult> HitResults;
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(ExplodeKnockBackRange);

	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Explosion), false, this);
	const FCollisionObjectQueryParams ObjectParams(HDCOLLISION_CHANNEL_PLAYER);

	World->SweepMultiByObjectType(
		HitResults,
		HitLocation,
		HitLocation,
		FQuat::Identity,
		ObjectParams,
		CollisionSphere,
		Params
	);

	TSet<AActor*> HitActors;
	for (const FHitResult& Result : HitResults)
	{
		AActor* Target = Result.GetActor();
		NULL_CHECK(Target);

		if (HitActors.Contains(Target))
		{
			continue;
		}

		HitActors.Add(Target);

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		NULL_CHECK(TargetASC);

		float Distance = FVector::Distance(HitLocation, Target->GetActorLocation());

		ApplyImpactGameAbility(TargetASC, KnockbackImpulse * (1.f - (Distance / ExplodeKnockBackRange)));

		if (Distance <= ExplodeDamageRange)
		{
			ApplyImpactGameEffect(TargetASC, ImpactDamage * (1.f - (Distance / ExplodeDamageRange)));
		}
	}
}

void AHDProjectileBase::ApplyImpactGameEffect(UAbilitySystemComponent* TargetAbiltySystemComponent, const float InterpImpactDamage)
{
	NULL_CHECK(ImpactGameEffect);
	VALID_CHECK(TargetAbiltySystemComponent);

	const FGameplayEffectContextHandle Context = TargetAbiltySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle ImpactGameEffectSpecHandle = TargetAbiltySystemComponent->MakeOutgoingSpec(ImpactGameEffect, 1.f, Context);
	CONDITION_CHECK(ImpactGameEffectSpecHandle.IsValid() == false);

	ImpactGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DAMAGE_PROJECTILE, -InterpImpactDamage);
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

void AHDProjectileBase::ApplyImpactGameAbility(UAbilitySystemComponent* TargetAbiltySystemComponent, const float InterpKnockbackImpulse)
{
	NULL_CHECK(TargetAbiltySystemComponent);

	CONDITION_CHECK(KnockbackTag.IsValid() == false);

	FGameplayEventData EventData;
	EventData.EventTag = KnockbackTag;
	EventData.Instigator = this;
	EventData.Target = TargetAbiltySystemComponent->GetOwnerActor();
	EventData.EventMagnitude = InterpKnockbackImpulse;

	TargetAbiltySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void AHDProjectileBase::ExcuteGameplayCue(UAbilitySystemComponent* OwnerAbilitySystemComponent, const FGameplayTag& Tag, const FHitResult& Hit)
{
	NULL_CHECK(OwnerAbilitySystemComponent);
	if (Tag.IsValid())
	{
		FGameplayCueParameters Params;
		Params.Location = Hit.ImpactPoint;
		Params.Normal = Hit.Normal;
		Params.Instigator = OwnerAbilitySystemComponent->GetOwnerActor();
		Params.SourceObject = this;
		OwnerAbilitySystemComponent->ExecuteGameplayCue(Tag, Params);
	}
}
