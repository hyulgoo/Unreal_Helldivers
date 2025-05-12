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

AHDProjectileBase::AHDProjectileBase()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    SetRootComponent(CollisionBox);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
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

    ProjectileMovementComponent->MaxSpeed = InitSpeed;
    ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = 0.f;

    StartDestroyTimer();
}

void AHDProjectileBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds); 
}

void AHDProjectileBase::Destroyed()
{
    UWorld* World = GetWorld();
    VALID_CHECK(World);

    UE_LOG(LogTemp, Warning, TEXT("DropTargetPosition : %s"), *GetActorLocation().ToString());
    
    Super::Destroyed();
}

void AHDProjectileBase::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    NULL_CHECK(OtherActor);

    // Get Target ASC
    IAbilitySystemInterface* TargetAbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor);
    if (TargetAbilitySystemInterface)
    {
        AActor* OwnerActor = GetOwner();
        NULL_CHECK(OwnerActor);

        IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerActor);
        NULL_CHECK(OwnerAbilitySystemInterface);

        UAbilitySystemComponent* SourceAbilitySystemComponent = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
        NULL_CHECK(SourceAbilitySystemComponent);

        UAbilitySystemComponent* TargetAbilitySystemComponent = TargetAbilitySystemInterface->GetAbilitySystemComponent();
        NULL_CHECK(TargetAbilitySystemComponent);

        NULL_CHECK(ImpactGameEffect);
        FGameplayEffectSpecHandle ImpactGameEffectSpecHandle = TargetAbilitySystemComponent->MakeOutgoingSpec(ImpactGameEffect, 1.f, TargetAbilitySystemComponent->MakeEffectContext());
        if (ImpactGameEffectSpecHandle.IsValid())
        {
            ImpactGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_PROJECTILE_DAMAGE, ImpactDamage);
            TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ImpactGameEffectSpecHandle.Data.Get());
        }

        if(StatusEffect != EStatusEffect::None)
        {
            FGameplayEffectSpecHandle StatusGameEffectSpecHandle = TargetAbilitySystemComponent->MakeOutgoingSpec(StatusGameEffect, 1.f, TargetAbilitySystemComponent->MakeEffectContext());
            if (StatusGameEffectSpecHandle.IsValid())
            {
                StatusGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_PROJECTILE_DOTDAMAGE, DotDamage);
                StatusGameEffectSpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_PROJECTILE_STATUSEFFECTDURATION, StatusDuration);
                TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*StatusGameEffectSpecHandle.Data.Get());
            }
        }
    }

    // TODO EffectCue 발동 (GAS가 없어도 일단 충돌했으면 Destroy)
    
    Destroy();
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
            FVector(1.f),
            EAttachLocation::KeepWorldPosition
        );
    }
}
