// Fill out your copyright notice in the Description page of Project Settings.

#include "Hellpod/HDHellpod.h"
#include "Define/HDDefine.h"
#include "Components/BoxComponent.h"

AHDHellpod::AHDHellpod()
{
	PrimaryActorTick.bCanEverTick = true;

	HellpodMesh = CreateDefaultSubobject<UStaticMeshComponent>("HellpodMesh");
	SetRootComponent(HellpodMesh);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll")); // TODO 프리셋 만들어서 설정
	CollisionComponent->OnComponentHit.AddDynamic(this, &AHDHellpod::OnBoxeHit);
}

void AHDHellpod::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDHellpod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHDHellpod::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHDHellpod::OnBoxeHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	NULL_CHECK(OtherActor);

	IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerActor);
	NULL_CHECK(OwnerAbilitySystemInterface);

	UAbilitySystemComponent* OwnerAbilitySystemComponent = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
	NULL_CHECK(OwnerAbilitySystemComponent);

	ExcuteGameplayCue(OwnerAbilitySystemComponent, ImpactBlocklCueTag, Hit);
}

void AHDHellpod::SpawnCharacter()
{
	NULL_CHECK(CharacterClass);

	UWorld* World = GetWorld();
	VALID_CHECK(World);

	AHDCharacterPlayer* CharacterPlayer = World->SpawnActor<AHDCharacterPlayer>(CharacterClass, GetActorTransform());
	NULL_CHECK(CharacterPlayer);

	AController* Controller = GetController();
	NULL_CHECK(Controller);
	Controller->Possess(Cast<APawn>(CharacterPlayer));
}
