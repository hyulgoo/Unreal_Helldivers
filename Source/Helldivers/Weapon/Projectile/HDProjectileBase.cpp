#include "HDProjectileBase.h"  
#include "Components/BoxComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "Kismet/GameplayStatics.h"  
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "Define/HDDefine.h"
#include "Define/HDGameplayTag.h"
#include "Collision/HDCollision.h"
#include "Weapon/WeaponTypes.h"

AHDProjectileBase::AHDProjectileBase()
	: ProjectileMesh(nullptr)
	, ProjectileMovement(nullptr)
	, CollisionBox(nullptr)
	, ProjectileTag(FGameplayTag())
	, InitSpeed(0.f)
	, ImpactType(EImpactType::Hit)
	, DamageGameEffect(nullptr)
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

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	ProjectileMovement->bRotationFollowsVelocity = true;
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

	TScriptInterface<IAbilitySystemInterface> OwnerAbilitySystemInterface = OwnerActor;
	NULL_CHECK(OwnerAbilitySystemInterface);

	UAbilitySystemComponent* OwnerASC = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
	NULL_CHECK(OwnerASC);

	if (ImpactType == EImpactType::Explode)
	{
		ApplyExplode(OwnerASC, Hit.ImpactPoint);
		ExecuteGameplayCue(OwnerASC, ImpactHitCueTag, Hit);
		//CreateCrater(OtherActor);
	}
	else
	{
		// Get Target ASC
		UAbilitySystemComponent* TargetASC = OtherActor->GetComponentByClass<UAbilitySystemComponent>();
		if (TargetASC)
		{
			ApplyDamageGameEffect(OwnerASC, TargetASC, ImpactDamage);
		}

		ExecuteGameplayCue(OwnerASC, TargetASC ? ImpactHitCueTag : ImpactBlocklCueTag, Hit);
	}

	Destroy();
}

void AHDProjectileBase::InitializeBeginPlay()
{
	CollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);

	ProjectileMovement->MaxSpeed = InitSpeed;
	ProjectileMovement->Velocity = GetActorForwardVector() * InitSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.f;
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

void AHDProjectileBase::ApplyExplode(UAbilitySystemComponent* SourceASC, const FVector& HitLocation)
{
	VALID_CHECK(SourceASC);

	UWorld* World = GetWorld();
	VALID_CHECK(World);

	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(ExplodeKnockBackRange);

	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Explosion), false, this);
	const FCollisionObjectQueryParams ObjectParams(ECC_Pawn);

	TArray<FHitResult> HitResults;
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

		const float Distance = FVector::Distance(HitLocation, Target->GetActorLocation());

		if (Distance <= ExplodeDamageRange)
		{
			const float Damage = ImpactDamage * (1.f - (Distance / ExplodeDamageRange));
			ApplyDamageGameEffect(SourceASC, TargetASC, Damage);
			UE_LOG(LogTemp, Error, TEXT("Distance : %f, ExplodeDamageRange : %f, ImpactDamage : %f"), Damage, ExplodeDamageRange, ImpactDamage);
		}

		ApplyKnockbackGameAbility(SourceASC, TargetASC, KnockbackImpulse * (1.f - (Distance / ExplodeKnockBackRange)));
	}
}

void AHDProjectileBase::ApplyDamageGameEffect(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC, const float InterpImpactDamage)
{
	NULL_CHECK(DamageGameEffect);
	VALID_CHECK(SourceASC);
	VALID_CHECK(TargetASC);

	const FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
	const FGameplayEffectSpecHandle ImpactGameEffectSpecHandle = TargetASC->MakeOutgoingSpec(DamageGameEffect, 1.f, Context);
	CONDITION_CHECK(ImpactGameEffectSpecHandle.IsValid() == false);

	ImpactGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DAMAGE_PROJECTILE, -InterpImpactDamage);
	SourceASC->ApplyGameplayEffectSpecToTarget(*ImpactGameEffectSpecHandle.Data.Get(), TargetASC);

	if (StatusEffect != EStatusEffect::None)
	{
		NULL_CHECK(StatusGameEffect);

		const FGameplayEffectSpecHandle StatusGameEffectSpecHandle = TargetASC->MakeOutgoingSpec(StatusGameEffect, 1.f, Context);
		CONDITION_CHECK(StatusGameEffectSpecHandle.IsValid() == false);

		StatusGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DOTDAMAGE_TICKDAMAGE, -DotDamage);
		StatusGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_DOTDAMAGE_DURATION, StatusDuration);
		SourceASC->ApplyGameplayEffectSpecToTarget(*StatusGameEffectSpecHandle.Data.Get(), TargetASC);
	}
}

void AHDProjectileBase::ApplyKnockbackGameAbility(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC, const float InterpKnockbackImpulse)
{
	NULL_CHECK(SourceASC);
	NULL_CHECK(TargetASC);

	CONDITION_CHECK(KnockbackTag.IsValid() == false);

	FGameplayEventData EventData;
	EventData.EventTag = KnockbackTag;
	EventData.Instigator = this;
	EventData.Target = TargetASC->GetOwnerActor();
	EventData.EventMagnitude = InterpKnockbackImpulse;

	TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void AHDProjectileBase::ExecuteGameplayCue(UAbilitySystemComponent* OwnerASC, const FGameplayTag& Tag, const FHitResult& Hit)
{
	NULL_CHECK(OwnerASC);

	if (Tag.IsValid())
	{
		FGameplayCueParameters Params;
		Params.Location = Hit.ImpactPoint;
		Params.Normal = Hit.Normal;
		Params.Instigator = OwnerASC->GetOwnerActor();
		Params.SourceObject = this;
		OwnerASC->ExecuteGameplayCue(Tag, Params);
	}
}
