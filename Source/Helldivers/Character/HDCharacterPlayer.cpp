
#include "Character/HDCharacterPlayer.h"
#include "Define/HDDefine.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterTypes/HDCharacterStateTypes.h"
#include "Component/Character/HDCombatComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Controller/HDPlayerController.h"
#include "Weapon/HDWeapon.h"
#include "Stratagem/HDStratagem.h"
#include "GameData/HDStratagemData.h"
#include "Animation/HDAnimInstance.h"
#include "Components/CapsuleComponent.h"

#define AIMOFFSET_PITCH_OFFSET 20.f
#define ANIMATIONNAME_RIFLE_AIM FName("Rifle_Aim")
#define ANIMATIONNAME_RIFLE_HIP FName("Rifle_Hip")
#define SOCKETNAME_RIGHTHAND FName("RightHandSocket")

AHDCharacterPlayer::AHDCharacterPlayer()
    : CameraBoom(nullptr)
	, FollowCamera(nullptr)
    , CurrentInputCommandList{}
	, Combat(nullptr)
	, Weapon(nullptr)
	, DefaultWeaponClass(nullptr)
    , DefaultCurve(nullptr)
	, StartingAimRotation(FRotator())
	, AimOffset_Yaw(0.f)
	, InterpAimOffset_Yaw(0.f)
	, AimOffset_Pitch(0.f)
	, bIsSprint(false)
    , bIsCrouch(false)
    , bIsProne(false)
	, bIsCharacterLookingViewport(false)
	, bUseRotateRootBone(false)
	, TurnThreshold(0.f)
	, TurningInPlace(EHDTurningInPlace::NotTurning)
	, StratagemClass(nullptr)
	, Stratagem(nullptr)
	, SelectedStratagemName(FName())
	, SelecteddStratagemActiveDelay(0.f)
    , CommandMatchStratagemNameList{}
	, AvaliableStratagemDataTable(nullptr)
	, DefaultFOV(0.f)
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // Camera
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Combat
    Combat = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));
    Combat->CombatState = EHDCombatState::Unoccupied;

    static ConstructorHelpers::FObjectFinder<UDataTable> StratagemDataListRef(TEXT("/Script/Engine.DataTable'/Game/Helldivers/GameData/DT_StratagenData.DT_StratagenData'"));
    if (StratagemDataListRef.Succeeded())
    {
        AvaliableStratagemDataTable = StratagemDataListRef.Object;
    }
}

void AHDCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AimOffset(DeltaTime);        
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SpawnDefaultWeapon();
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    SetProne(true);
}

void AHDCharacterPlayer::SetWeaponActive(const bool bActive)
{
    if (IsValid(Weapon))
    {
        UHDAnimInstance* HDCharacterInstance = Cast<UHDAnimInstance>(GetMesh()->GetAnimInstance());
        NULL_CHECK(HDCharacterInstance);

        HDCharacterInstance->SetUseUpperSlot(bActive == false);

        Weapon->SetActorTickEnabled(bActive);
        Weapon->SetActorHiddenInGame(bActive == false);
        Weapon->SetActorEnableCollision(bActive);
    }  
}

const bool AHDCharacterPlayer::IsShouldering() const
{
    return Combat->bIsShoulder;
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    // 오버라이드하여 실제 CameraSpringArmLength 조정 중
    Combat->bIsShoulder = bShoulder;   
}

void AHDCharacterPlayer::EquipWeapon(AHDWeapon* NewWeapon)
{
    VALID_CHECK(NewWeapon);
    Weapon = NewWeapon;

    Combat->EquipWeapon(NewWeapon);
    Weapon->SetOwner(this);

    const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(SOCKETNAME_RIGHTHAND);
    NULL_CHECK(RightHandSocket);
    RightHandSocket->AttachActor(Weapon, GetMesh());
}

AHDWeapon* AHDCharacterPlayer::GetWeapon() const
{
    return Weapon;
}

const FVector& AHDCharacterPlayer::GetHitTarget() const
{
    return Combat->HitTarget;
}

const EHDCombatState AHDCharacterPlayer::GetCombatState() const
{
    return Combat->CombatState;
}

void AHDCharacterPlayer::PlayFireMontage(const bool bAiming)
{
    VALID_CHECK(Weapon);

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    NULL_CHECK(AnimInstance);
    NULL_CHECK(FireWeaponMontage);

    AnimInstance->Montage_Play(FireWeaponMontage);
    const FName SectionName = Combat->bIsShoulder ? ANIMATIONNAME_RIFLE_AIM : ANIMATIONNAME_RIFLE_HIP;
    AnimInstance->Montage_JumpToSection(SectionName);
}

void AHDCharacterPlayer::AddStratagemCommand(const EHDCommandInput NewInput)
{
    CurrentInputCommandList.Add(NewInput);
    CommandMatchStratagemNameList.Empty();
    const TArray<FName> StratagemDataNameList = AvaliableStratagemDataTable->GetRowNames();

    const FString FindString(TEXT("Lookup"));
    for (const FName& StratagemDataName : StratagemDataNameList)
    {
        FHDStratagemData* StratagemData = AvaliableStratagemDataTable->FindRow<FHDStratagemData>(StratagemDataName, FindString);
        const TArray<EHDCommandInput>& CommandList = StratagemData->CommandSequence;
        if (CurrentInputCommandList.Num() > CommandList.Num())
        {
            continue;
        }

        bool bCommandMatch = true;
        for (int32 index = 0; index < CurrentInputCommandList.Num(); ++index)
        {
            if (CurrentInputCommandList[index] != CommandList[index])
            {
                bCommandMatch = false;
                break;
            }
        }

        if(bCommandMatch)
        {
            if (CurrentInputCommandList.Num() == CommandList.Num())
            {
                SelectedStratagemName = StratagemDataName;
                SelecteddStratagemActiveDelay = StratagemData->StratagemDelay;
            }

            CommandMatchStratagemNameList.Add(StratagemDataName);
        }
    }

    AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);
    PlayerController->SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputCommandList.Num());
}

void AHDCharacterPlayer::SetSprint(const bool bSprint)
{
    // 해당 클래스를 상속받은 캐릭터에서 해당 함수 Override하여 스피드 조정 중
    bIsSprint = bSprint;
}

void AHDCharacterPlayer::SetCrouch(const bool bCrouch)
{
    bIsCrouch = bCrouch;
}

void AHDCharacterPlayer::SetProne(const bool bProne)
{
	bIsProne = bProne;
}

const FVector& AHDCharacterPlayer::GetThrowDirection() const
{
    return Combat->HitTarget;
}

void AHDCharacterPlayer::ThrowStratagem()
{   
    USkeletalMeshComponent* CharacterMesh = GetMesh();
    UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
    NULL_CHECK(AnimInstance);
    NULL_CHECK(ThrowMontage);
    NULL_CHECK(StratagemClass);

    if (SelectedStratagemName.IsNone() == false)
    {
        SetWeaponActive(false);

        // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
        PlayThrowMontage();

        UWorld* World = GetWorld();
        VALID_CHECK(World);

        const USkeletalMeshSocket* RightHandSocket = CharacterMesh->GetSocketByName(SOCKETNAME_RIGHTHAND);
        NULL_CHECK(RightHandSocket);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = CastChecked<APawn>(this);

        const FTransform SocketTransform = RightHandSocket->GetSocketTransform(CharacterMesh);
        FRotator TargetRotation = (Combat->HitTarget - SocketTransform.GetLocation()).Rotation();
        TargetRotation.Roll = 0;
        Stratagem = World->SpawnActor<AHDStratagem>(StratagemClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
        RightHandSocket->AttachActor(Stratagem, CharacterMesh);
        Stratagem->SetOwner(this);
        Stratagem->StratagemName = SelectedStratagemName;
        Stratagem->StratagemActiveDelay = SelecteddStratagemActiveDelay;
    }

    SelectedStratagemName = FName();
    CommandMatchStratagemNameList.Empty();
    CurrentInputCommandList.Empty();
}

void AHDCharacterPlayer::AimOffset(const float DeltaTime)
{
    if (IsValid(Weapon) == false)
    {
        return;
    }

    UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();

    FVector Velocity = GetVelocity();
    Velocity.Z = 0.f;
    const bool bIsMoving            = Velocity.Size() > 0.1f;
    const bool bIsFalling           = CharacterMovementComponent->IsFalling();
    const FRotator BaseAimRoatation = GetBaseAimRotation();
    const FRotator ControlRotation  = GetControlRotation();

    if (Combat->bIsShoulder)
    {
        bIsCharacterLookingViewport = true;
        bUseControllerRotationYaw = true;
        bUseRotateRootBone = false;
        CharacterMovementComponent->bOrientRotationToMovement = false;

        const FRotator TargetRotation(0.f, ControlRotation.Yaw, 0.f);
        const FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 20.f);
        SetActorRotation(SmoothRotation);

        if (bIsMoving == false && bIsFalling == false)
        {
            const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, StartingAimRotation);
            AimOffset_Yaw = DeltaRotation.Yaw;

            if (TurningInPlace == EHDTurningInPlace::NotTurning)
            {
                InterpAimOffset_Yaw = AimOffset_Yaw;
            }

            TurnInPlace(DeltaTime);
        }

        if (bIsMoving || bIsFalling)
        {
            AimOffset_Yaw = 0.f;
            InterpAimOffset_Yaw = 0.f;
            StartingAimRotation = TargetRotation;
        }
    }
    else
    {
        const FRotator TargetRotation(0.f, BaseAimRoatation.Yaw, 0.f);

        bIsCharacterLookingViewport = true;
        bUseRotateRootBone = true;
        bUseControllerRotationYaw = false;
        CharacterMovementComponent->bOrientRotationToMovement = false;
        const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, StartingAimRotation);
        AimOffset_Yaw = DeltaRotation.Yaw;

        if (bIsMoving == false && bIsFalling == false)
        {
            if (TurningInPlace == EHDTurningInPlace::NotTurning)
            {
                InterpAimOffset_Yaw = AimOffset_Yaw;
            }

            bUseRotateRootBone = false;
            bUseControllerRotationYaw = true;
            TurnInPlace(DeltaTime);
        }

        if (bIsMoving || bIsFalling)
        {
            bIsCharacterLookingViewport = false;
            bUseRotateRootBone = false;
            CharacterMovementComponent->bOrientRotationToMovement = true;
            AimOffset_Yaw = 0.f;
            StartingAimRotation = BaseAimRoatation;
        }
    }

    CalculationAimOffset_Pitch();
}

void AHDCharacterPlayer::TurnInPlace(float DeltaTime)
{
    if (AimOffset_Yaw > TurnThreshold)
    {
        TurningInPlace = EHDTurningInPlace::Turn_Right;
    }
    else if (AimOffset_Yaw < -TurnThreshold)
    {
        TurningInPlace = EHDTurningInPlace::Turn_Left;
    }

    if (TurningInPlace != EHDTurningInPlace::NotTurning)
    {
        InterpAimOffset_Yaw = FMath::FInterpTo(InterpAimOffset_Yaw, 0.f, DeltaTime, 4.f);
        AimOffset_Yaw = InterpAimOffset_Yaw;
        if (FMath::Abs(AimOffset_Yaw) < 15.f)
        {
            StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
            TurningInPlace = EHDTurningInPlace::NotTurning;
        }
    }
}

void AHDCharacterPlayer::CalculationAimOffset_Pitch()
{
    AimOffset_Pitch= GetBaseAimRotation().Pitch - AIMOFFSET_PITCH_OFFSET;
}

void AHDCharacterPlayer::SpawnDefaultWeapon()
{
    NULL_CHECK(DefaultWeaponClass);

    UWorld* World = GetWorld();
    VALID_CHECK(World);

    Weapon = World->SpawnActor<AHDWeapon>(DefaultWeaponClass);
    NULL_CHECK(Weapon);

    EquipWeapon(Weapon);
}

void AHDCharacterPlayer::InterpFOV(float DeltaSeconds)
{
    if (IsValid(Weapon) == false)
    {
        return;
    }

    const float TargetFOV = Combat->bIsShoulder ? Weapon->GetZoomedFOV() : DefaultFOV;
    const float InterpSpeed = Combat->bIsShoulder ? Weapon->GetZoomInterpSpeed() : Combat->ZoomInterpSpeed;
    Combat->CurrentFOV = FMath::FInterpTo(Combat->CurrentFOV, TargetFOV, DeltaSeconds, InterpSpeed);
    
    FollowCamera->SetFieldOfView(Combat->CurrentFOV);
}

void AHDCharacterPlayer::Fire(const bool IsPressed)
{
    if(Combat->Fire(IsPressed) == false)
    {
        return;
    }

    switch (Weapon->GetFireType())
    {
    case EFireType::HitScan:
    case EFireType::Projectile:
    {
        PlayFireMontage(Combat->bIsShoulder);

        AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
        NULL_CHECK(PlayerController);
        PlayerController->ChangeAmmoHUDInfo(Weapon->GetAmmoCount());
    }
    break;
    case EFireType::Shotgun:
    {
        // TO DO
        //PlayFireMontage(bIsShoulder);
        //Shotgun->FireShotgun(TraceHitTargets);
        //CombatState = ECombatState::ECS_Unoccupied;
    }
    break;
    }
}

void AHDCharacterPlayer::PlayThrowMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    VALID_CHECK(AnimInstance);
    NULL_CHECK(ThrowMontage);

    AnimInstance->Montage_Play(ThrowMontage);
}

void AHDCharacterPlayer::SetDead()
{
    Super::SetDead();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    NULL_CHECK(PlayerController);

    if (IsLocallyControlled())
    {
        DisableInput(PlayerController);
    }
}
