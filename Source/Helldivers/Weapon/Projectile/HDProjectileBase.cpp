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
#include "LandScape.h"
#include "LandScapeEdit.h"
#include "EngineUtils.h"

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

	static ConstructorHelpers::FClassFinder<UGameplayEffect> DamageGamEffectRef(TEXT("'/Game/Helldivers/Blueprint/GameAbility/_Effect/BP_GE_ApplyDamage.BP_GE_ApplyDamage_C'"));
	if (DamageGamEffectRef.Succeeded())
	{
		DamageGameEffect = DamageGamEffectRef.Class;
	}
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
		ExcuteGameplayCue(OwnerASC, ImpactHitCueTag, Hit);
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

		ExcuteGameplayCue(OwnerASC, TargetASC ? ImpactHitCueTag : ImpactBlocklCueTag, Hit);
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

void AHDProjectileBase::ExcuteGameplayCue(UAbilitySystemComponent* OwnerASC, const FGameplayTag& Tag, const FHitResult& Hit)
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

void AHDProjectileBase::CreateCrater(AActor* OtherActor)
{
	ALandscape* LandscapeActor = nullptr;
	for (TActorIterator<ALandscape> It(GetWorld()); It; ++It)
	{
		LandscapeActor = *It;
		break;
	}

	if (LandscapeActor == nullptr)
	{
		return;
	}

	ULandscapeInfo* LandscapeInfo = LandscapeActor->GetLandscapeInfo();
	NULL_CHECK(LandscapeInfo);

	TArray<FIntPoint> Keys;
	LandscapeInfo->XYtoComponentMap.GetKeys(Keys);
	if (Keys.Num() == 0) return;

	FLandscapeEditDataInterface LandscapeEdit(LandscapeInfo);
	FIntPoint MinKey = Keys[0], MaxKey = Keys[0];
	for (auto& K : Keys)
	{
		MinKey.X = FMath::Min(MinKey.X, K.X);
		MinKey.Y = FMath::Min(MinKey.Y, K.Y);
		MaxKey.X = FMath::Max(MaxKey.X, K.X);
		MaxKey.Y = FMath::Max(MaxKey.Y, K.Y);
	}
	// 한 컴포넌트당 Quad 수
	int32 QuadsPerComponent = LandscapeInfo->ComponentSizeQuads * LandscapeInfo->ComponentNumSubsections;

	// 4) 월드 → Heightmap 인덱스로 변환 람다
	auto WorldToDataXY = [&](const FVector& WP, int32& OutX, int32& OutY)
		{
			// Landscape 로컬 공간으로
			FVector Local = LandscapeActor->GetTransform().InverseTransformPosition(WP);
			// DrawScale 분할
			OutX = FMath::RoundToInt(Local.X / LandscapeInfo->DrawScale.X);
			OutY = FMath::RoundToInt(Local.Y / LandscapeInfo->DrawScale.Y);
			// 전체 범위에 클램프
			OutX = FMath::Clamp(OutX, MinKey.X, MaxKey.X + QuadsPerComponent);
			OutY = FMath::Clamp(OutY, MinKey.Y, MaxKey.Y + QuadsPerComponent);
		};

	const FVector& ActorLocation = GetActorLocation();

	// 5) 수정할 영역 계산
	int32 MinX, MinY, MaxX, MaxY;
	WorldToDataXY(ActorLocation - FVector(ExplodeDamageRange, ExplodeDamageRange, 0), MinX, MinY);
	WorldToDataXY(ActorLocation + FVector(ExplodeDamageRange, ExplodeDamageRange, 0), MaxX, MaxY);

	int32 SizeX = MaxX - MinX + 1;
	int32 SizeY = MaxY - MinY + 1;
	if (SizeX <= 0 || SizeY <= 0) return;

	// 6) 기존 높이 읽기
	TArray<uint16> OrigHeights;
	OrigHeights.SetNum(SizeX * SizeY);
	LandscapeEdit.GetHeightDataFast(MinX, MinY, MaxX, MaxY, OrigHeights.GetData(), 0);

	// 7) 새 높이 계산
	TArray<uint16> NewHeights;
	NewHeights.SetNum(SizeX * SizeY);

	float InvZ = 1.f / LandscapeInfo->DrawScale.Z;
	for (int32 Y = 0; Y < SizeY; ++Y)
	{
		for (int32 X = 0; X < SizeX; ++X)
		{
			int32 I = X + Y * SizeX;
			// 월드 거리 계산
			float WX = (MinX + X) * LandscapeInfo->DrawScale.X;
			float WY = (MinY + Y) * LandscapeInfo->DrawScale.Y;
			float Dist = FVector2D(WX - ActorLocation.X, WY - ActorLocation.Y).Size();
			float Atten = FMath::Clamp(1.f - Dist / ExplodeDamageRange, 0.f, 1.f);

			float HF = OrigHeights[I] - ExplodeDamageRange * Atten * InvZ;
			NewHeights[I] = FMath::Clamp<int32>(FMath::RoundToInt(HF), 0, MAX_uint16);
		}
	}

	// 8) Heightmap 쓰기 & Flush
	LandscapeEdit.SetHeightData(MinX, MinY, MaxX, MaxY, NewHeights.GetData(), 0, true);
	LandscapeEdit.Flush();
}
