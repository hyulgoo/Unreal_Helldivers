
#include "Character/HDCharacterPlayer.h"
#include "Define/HDDefine.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Component/Character/HDCombatComponent.h"
#include "CharacterTypes/HDCharacterStateTypes.h"
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
#include "Character/HDCharacterControlData.h"

#define AIMOFFSET_PITCH_OFFSET 20.f
#define MONTAGESECTIONNAME_RIFLE_AIM FName("Rifle_Aim")
#define MONTAGESECTIONNAME_RIFLE_HIP FName("Rifle_Hip")
#define MONTAGESECTIONNAME_PISTOL FName("Pistol")
#define MONTAGESECTIONNAME_SHOTGUN_AIM FName("Shotgun_Aim")
#define MONTAGESECTIONNAME_SHOTGUN_HIP FName("Shotgun_Hip")
#define MONTAGESECTIONNAME_RIFLE_PRONE FName("Rifle_Prone")
#define MONTAGESECTIONNAME_SHOTGUN_PRONE FName("Shotgun_Prone")
#define SOCKETNAME_RIGHTHAND FName("RightHandSocket")

AHDCharacterPlayer::AHDCharacterPlayer()
    : CameraBoom(nullptr)
	, FollowCamera(nullptr)
    , CurrentInputCommandList{}
	, Combat(nullptr)
	, DefaultWeaponClass(nullptr)
	, StartingAimRotation(FRotator())
	, AimOffset_Yaw(0.f)
	, InterpAimOffset_Yaw(0.f)
	, AimOffset_Pitch(0.f)
    , MovementState(EHDCharacterMovementState::Idle)
    , PrevMovementState(EHDCharacterMovementState::Idle)
	, bIsSprint(false)
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
    , FireTimer(FTimerHandle())
    , ReloadTimer(FTimerHandle())
    , DefaultCurve(nullptr)
    , ArmLengthTimeline(FTimeline())
    , TargetArmLength(0.f)
    , CameraTargetZOffset(0.f)
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
    ArmLengthTimeline.TickTimeline(DeltaTime);
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SpawnDefaultWeapon();

    FOnTimelineFloat ArmLengthTimelineProgress;
    ArmLengthTimelineProgress.BindUFunction(this, FName("OnCameraSpringArmLengthTImelineUpdate"));
    ArmLengthTimeline.AddInterpFloat(DefaultCurve, ArmLengthTimelineProgress);
    ArmLengthTimeline.SetLooping(false);
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    SetCharacterMovementState(EHDCharacterMovementState::Prone, true);
}

void AHDCharacterPlayer::SetWeaponActive(const bool bActive)
{
    AHDWeapon* Weapon = GetWeapon();
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

void AHDCharacterPlayer::Reload()
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK(Weapon);

    if (Combat->CanReload() == false)
    {
        return;
    }

	Combat->Reload();

	FName SectionName;
	switch (Weapon->GetFireType())
	{
	case EHDFireType::HitScan:
	case EHDFireType::Projectile:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? MONTAGESECTIONNAME_RIFLE_PRONE
			: Combat->bIsShoulder ? MONTAGESECTIONNAME_RIFLE_AIM : MONTAGESECTIONNAME_RIFLE_HIP;
		break;
	case EHDFireType::Shotgun:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? MONTAGESECTIONNAME_SHOTGUN_PRONE
			: Combat->bIsShoulder ? MONTAGESECTIONNAME_SHOTGUN_AIM : MONTAGESECTIONNAME_SHOTGUN_HIP;
		break;
	}

	CONDITION_CHECK(SectionName.IsNone());

	PlayMontage(ReloadWeaponMontage, SectionName);

    GetWorldTimerManager().SetTimer(ReloadTimer, this, &AHDCharacterPlayer::ReloadTimerFinished, Weapon->GetReloadDelay(Combat->bIsShoulder));
}

const bool AHDCharacterPlayer::IsShouldering() const
{
    return Combat->bIsShoulder;
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    Combat->bIsShoulder = bShoulder;   

    if (bShoulder)
    {
        ArmLengthTimeline.PlayFromStart();
    }
    else
    {
        ArmLengthTimeline.ReverseFromEnd();
    }
}

void AHDCharacterPlayer::EquipWeapon(AHDWeapon* NewWeapon)
{
    VALID_CHECK(NewWeapon);

    NewWeapon->SetOwner(this);
    Combat->EquipWeapon(NewWeapon);

    const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(SOCKETNAME_RIGHTHAND);
    NULL_CHECK(RightHandSocket);
    RightHandSocket->AttachActor(NewWeapon, GetMesh());
}

AHDWeapon* AHDCharacterPlayer::GetWeapon() const
{
    return Combat->Weapon;
}

const FVector& AHDCharacterPlayer::GetHitTarget() const
{
    return Combat->HitTarget;
}

const EHDCombatState AHDCharacterPlayer::GetCombatState() const
{
    return Combat->CombatState;
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

    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK(PlayerController);
    PlayerController->SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputCommandList.Num());
}

void AHDCharacterPlayer::SetCharacterControlData(UHDCharacterControlData* CharacterControlData)
{
    bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

    UCharacterMovementComponent* CharacterMovementComponent     = GetCharacterMovement();
    CharacterMovementComponent->bOrientRotationToMovement       = CharacterControlData->bOrientRotationToMovement;
    CharacterMovementComponent->bUseControllerDesiredRotation   = CharacterControlData->bUseControllerDesiredRotation;
    CharacterMovementComponent->RotationRate                    = CharacterControlData->RotationRate;

    CameraTargetZOffset                 = CharacterControlData->TargetOffset.Z;
    TargetArmLength                     = CharacterControlData->TargetArmLength;
    CameraBoom->TargetArmLength         = CharacterControlData->TargetArmLength;
    CameraBoom->TargetOffset            = CharacterControlData->TargetOffset;
    CameraBoom->SocketOffset            = CharacterControlData->SocketOffset;
    CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
    CameraBoom->bInheritPitch           = CharacterControlData->bInheritPitch;
    CameraBoom->bInheritYaw             = CharacterControlData->bInheritYaw;
    CameraBoom->bInheritRoll            = CharacterControlData->bInheritRoll;
    CameraBoom->bDoCollisionTest        = CharacterControlData->bDoCollisionTest;
}

void AHDCharacterPlayer::OnCameraSpringArmLengthTImelineUpdate(const float Value)
{
    const float Interpolated = FMath::Lerp(TargetArmLength, TargetArmLength / 2.f, Value);
    CameraBoom->TargetArmLength = Interpolated;
}

void AHDCharacterPlayer::SetSprint(const bool bSprint)
{
    // 해당 클래스를 상속받은 캐릭터에서 해당 함수 Override하여 스피드 조정 중
    bIsSprint = bSprint;
}

const EHDCharacterMovementState AHDCharacterPlayer::GetCharacterMovementState() const
{
    return MovementState;
}

void AHDCharacterPlayer::SetCharacterMovementState(const EHDCharacterMovementState NewState, const bool bForced)
{
    CONDITION_CHECK(NewState == EHDCharacterMovementState::Count);

    PrevMovementState = MovementState;
    MovementState = NewState;
	if (bForced)
	{
        Combat->CombatState = NewState == EHDCharacterMovementState::Prone ? EHDCombatState::Ragdoll : EHDCombatState::Unoccupied;
    }

    ChangeCameraZOffsetByCharacterMovementState(MovementState);
}

void AHDCharacterPlayer::RestoreMovementState()
{
    MovementState = MovementState != EHDCharacterMovementState::Prone ? PrevMovementState
        : EHDCharacterMovementState::Idle;

    ChangeCameraZOffsetByCharacterMovementState(MovementState);
}

const FVector& AHDCharacterPlayer::GetThrowDirection() const
{
    return Combat->HitTarget;
}

void AHDCharacterPlayer::ThrowStratagem()
{   
    NULL_CHECK(ThrowMontage);
    NULL_CHECK(StratagemClass);

    if (SelectedStratagemName.IsNone() == false)
    {
        SetWeaponActive(false);

        // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
        PlayMontage(ThrowMontage);

        UWorld* World = GetWorld();
        VALID_CHECK(World);

        USkeletalMeshComponent* CharacterMesh = GetMesh();
        const USkeletalMeshSocket* RightHandSocket = CharacterMesh->GetSocketByName(SOCKETNAME_RIGHTHAND);
        NULL_CHECK(RightHandSocket);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Cast<APawn>(this);

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
    if (IsValid(GetWeapon()) == false)
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
        bIsCharacterLookingViewport = true;
        bUseRotateRootBone = true;
        bUseControllerRotationYaw = false;
        CharacterMovementComponent->bOrientRotationToMovement = false;
        const FRotator TargetRotation(0.f, BaseAimRoatation.Yaw, 0.f);
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

    AHDWeapon* Weapon = World->SpawnActor<AHDWeapon>(DefaultWeaponClass);
    NULL_CHECK(Weapon);

    EquipWeapon(Weapon);
}

void AHDCharacterPlayer::InterpFOV(float DeltaSeconds)
{
    AHDWeapon* Weapon = GetWeapon();
    if (IsValid(Weapon) == false)
    {
        return;
    }

    const float TargetFOV = Combat->bIsShoulder ? Weapon->GetZoomedFOV() : DefaultFOV;
    const float InterpSpeed = Combat->bIsShoulder ? Weapon->GetZoomInterpSpeed() : Combat->ZoomInterpSpeed;
    Combat->CurrentFOV = FMath::FInterpTo(Combat->CurrentFOV, TargetFOV, DeltaSeconds, InterpSpeed);
    
    FollowCamera->SetFieldOfView(Combat->CurrentFOV);
}

void AHDCharacterPlayer::PlayMontage(UAnimMontage* Montage, const FName SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    VALID_CHECK(AnimInstance);
    NULL_CHECK(Montage);

    AnimInstance->Montage_Play(Montage);

    if (SectionName.IsValid())
    {
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void AHDCharacterPlayer::FireTimerFinished()
{
    if (Combat->FireTimerFinished())
    {
        Fire(true);
    }
}

void AHDCharacterPlayer::ReloadTimerFinished()
{
    Combat->ReloadTimerFinished();

    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK(PlayerController);

    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK(Weapon);
    PlayerController->ChangeCapacityHUDInfo(Weapon->GetCapacityCount());
}

void AHDCharacterPlayer::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterMovementState State)
{
    switch (State)
    {
    case EHDCharacterMovementState::Idle:
        CameraBoom->TargetOffset.Z = CameraTargetZOffset;
        break;
    case EHDCharacterMovementState::Crouch:
        CameraBoom->TargetOffset.Z = CameraTargetZOffset - 40.f;
        break;
    case EHDCharacterMovementState::Prone:
        CameraBoom->TargetOffset.Z = CameraTargetZOffset - 80.f;
        break;
    default:
        LOG("EHDCharacterMovementState is Invalid!!");
        break;
    }
}

void AHDCharacterPlayer::Fire(const bool IsPressed)
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK(Weapon);

    if(Combat->Fire(IsPressed) == false)
    {
        if (Combat->CanReload() && Weapon->IsAmmoEmpty())
        {
            Reload();
        }

        return;
    }

    switch (Weapon->GetFireType())
    {
    case EHDFireType::HitScan:
    case EHDFireType::Projectile:
    {
        const FName SectionName = Combat->bIsShoulder ? MONTAGESECTIONNAME_RIFLE_AIM : MONTAGESECTIONNAME_RIFLE_HIP;
        PlayMontage(FireWeaponMontage, SectionName);

		AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
		NULL_CHECK(PlayerController);
		PlayerController->ChangeAmmoHUDInfo(Weapon->GetAmmoCount());

        GetWorldTimerManager().SetTimer(FireTimer, this, &AHDCharacterPlayer::FireTimerFinished, Weapon->GetFireDelay());
    }
    break;
    case EHDFireType::Shotgun:
    {
        // TO DO
        //const FName SectionName = Combat->bIsShoulder ? MONTAGESECTIONNAME_SHOTGUN_AIM : MONTAGESECTIONNAME_SHOTGUN_HIP;
        //PlayMontage(FireWeaponMontage);
        //Shotgun->FireShotgun(TraceHitTargets);
        //CombatState = ECombatState::ECS_Unoccupied;
    }
    break;
    }
}

void AHDCharacterPlayer::SetDead()
{
    Super::SetDead();

    APlayerController* PlayerController = GetController<APlayerController>();
    NULL_CHECK(PlayerController);

    if (IsLocallyControlled())
    {
        DisableInput(PlayerController);
    }
}
