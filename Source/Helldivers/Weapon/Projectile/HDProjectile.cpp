#include "HDProjectile.h"  
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
#include "GameData/HDProjectileData.h"

AHDProjectile::AHDProjectile()
    : Damage(0.f)
    , HeadShotDamageRate(0.f)
    , InitialSpeed(15000.f)
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
        UE_LOG(LogTemp, Warning, TEXT("DropTargetPosition : %s"), *GetActorLocation().ToString());
    }

    Super::Destroyed();
}

void AHDProjectile::OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    NULL_CHECK(ProjectileDataTable);
    NULL_CHECK(OtherActor);

    static const FString ContextString(TEXT("ProjectileDataLookup"));
    FHDProjectileData* DataRow = ProjectileDataTable->FindRow<FHDProjectileData>(ProjectileTag.GetTagName(), ContextString);
    NULL_CHECK(DataRow);

    // Get Owner ASC
    AActor* OwnerActor = GetOwner();
    NULL_CHECK(OwnerActor);
    IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerActor);
    if(OwnerAbilitySystemInterface == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* SourceAbilitySystemComponent = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
    NULL_CHECK(SourceAbilitySystemComponent);

    // Get Target ASC
    IAbilitySystemInterface* TargetAbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor);
    if (TargetAbilitySystemInterface == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* TargetAbilitySystemComponent = TargetAbilitySystemInterface->GetAbilitySystemComponent();
    NULL_CHECK(TargetAbilitySystemComponent);

    
    for (auto ImpactGameEffect : DataRow->ImpactGameEffectList) 
    {

    }

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
