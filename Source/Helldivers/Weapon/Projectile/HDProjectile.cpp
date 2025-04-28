#include "HDProjectile.h"  
#include "Components/BoxComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "NiagaraComponent.h"  
#include "Kismet/GameplayStatics.h"  
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/GameAbility/HDGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Attribute/HDCharacterAttributeSet.h"
#include "GameAbility/Effect/HDGE_FireDamage.h"

AHDProjectile::AHDProjectile()
    : Damage(0.f)
    , HeadShotDamageRate(0.f)
    , InitialSpeed(15000.f)
    , ImpactParticlesScale(1.f)
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    SetRootComponent(CollisionBox);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    CollisionBox->OnComponentHit.AddDynamic(this, &AHDProjectile::OnBoxHit);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
    ProjectileMovementComponent->bRotationFollowsVelocity = false;

   static ConstructorHelpers::FClassFinder<UHDGE_FireDamage> FireDamageEffectRef(TEXT("/Game/Helldivers/Blueprint/GameAbility/Effect/BP_GE_FireDamage.BP_GE_FireDamage_C"));
   if (FireDamageEffectRef.Class)
   {
       FireDamageEffect = FireDamageEffectRef.Class;
   }
}

void AHDProjectile::BeginPlay()
{
    Super::BeginPlay();

    if(abs(InitialSpeed) < 0.1f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Set Projectile SpawnActorDeferred!"));
    }

    ProjectileMovementComponent->MaxSpeed = InitialSpeed;
    ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitialSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = 0.f;

    SpawnTrailSystem();
}

void AHDProjectile::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AHDProjectile::Destroyed()
{
    UWorld* World = GetWorld();
    if (World)
    {
        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, GetActorTransform());
        }

        if (ImpactSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
        }

        UE_LOG(LogTemp, Warning, TEXT("DropTargetPosition : %s"), *GetActorLocation().ToString());
    }

    Super::Destroyed();
}

void AHDProjectile::SpawnTrailSystem()
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
            FVector(ImpactParticlesScale, ImpactParticlesScale, ImpactParticlesScale),
            EAttachLocation::KeepWorldPosition
        );
    }
}

void AHDProjectile::ExplodeDamage()
{
   APawn* FiringPawn = GetInstigator();
   if (FiringPawn)
   {
       AController* FiringController = FiringPawn->GetController();
       if (FiringController)
       {
           UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 10.f, GetActorLocation(), DamageInnerRadius, DamageOuterRadius, 1.f,
               UDamageType::StaticClass(), TArray<AActor*>(), this, FiringController);
       }
   }
}

void AHDProjectile::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AHDGASCharacterPlayer* TargetGASPlayer = Cast<AHDGASCharacterPlayer>(OtherActor);
    if (TargetGASPlayer)
    {
        UAbilitySystemComponent* TargetASC = TargetGASPlayer->GetAbilitySystemComponent();
        if (TargetASC && FireDamageEffect)
        {
            FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(FireDamageEffect, 1.f, TargetASC->MakeEffectContext());
            if (EffectSpecHandle.IsValid())
            {
                FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
                if (EffectSpec)
                {
                    EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Damage")), Damage);
                    TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
                }
            }
        }
    }

    Destroy();
}

void AHDProjectile::StartDestroyTimer()
{
    if (GetWorld())
    {
        GetWorldTimerManager().SetTimer(DestroyTimer, this, &AHDProjectile::DestroyTimerFinished, DestroyTime, false);
    }
}

void AHDProjectile::DestroyTimerFinished()
{
    Destroy();
}
