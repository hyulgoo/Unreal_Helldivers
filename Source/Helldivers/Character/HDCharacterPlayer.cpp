
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
    : SpringArm(nullptr)
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
    , SpringArmArmLengthTimeline(FTimeline())
    , SpringArmTargetArmLength(0.f)
    , SpringArmTargetZOffset(0.f)
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // Camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Combat
    Combat = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));

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
    SpringArmArmLengthTimeline.TickTimeline(DeltaTime);
}

void AHDCharacterPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SpawnDefaultWeapon();

    FOnTimelineFloat ArmLengthTimelineProgress;
    ArmLengthTimelineProgress.BindUFunction(this, FName("OnCameraSpringArmLengthTimelineUpdate"));
    SpringArmArmLengthTimeline.AddInterpFloat(DefaultCurve, ArmLengthTimelineProgress);
    SpringArmArmLengthTimeline.SetLooping(false);
}

void AHDCharacterPlayer::SetRagdoll(const bool bRagdoll, const FVector& Impulse)
{
    Super::SetRagdoll(bRagdoll, Impulse);

    if (bRagdoll == false)
    {
        SetCharacterMovementState(EHDCharacterMovementState::Prone, true);

        FTimerDelegate LamdaDelegate;
        LamdaDelegate.BindLambda([this]() {
            SetCombatState(EHDCombatState::Unoccupied);
            });
        GetWorldTimerManager().SetTimerForNextTick(LamdaDelegate);
    }
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

const float AHDCharacterPlayer::Reload()
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    if (Combat->CanReload() == false)
    {
        return 0.f;
    }

	Combat->Reload();

    const bool bIsShoulder = IsShouldering();

	FName SectionName;
	switch (Weapon->GetFireType())
	{
	case EHDFireType::HitScan:
	case EHDFireType::Projectile:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? MONTAGESECTIONNAME_RIFLE_PRONE
			: bIsShoulder ? MONTAGESECTIONNAME_RIFLE_AIM : MONTAGESECTIONNAME_RIFLE_HIP;
		break;
	case EHDFireType::Shotgun:
		SectionName = MovementState == EHDCharacterMovementState::Prone ? MONTAGESECTIONNAME_SHOTGUN_PRONE
			: bIsShoulder ? MONTAGESECTIONNAME_SHOTGUN_AIM : MONTAGESECTIONNAME_SHOTGUN_HIP;
		break;
	}

	CONDITION_CHECK_WITH_RETURNTYPE(SectionName.IsNone(), 0.f);

	PlayMontage(ReloadWeaponMontage, SectionName);

    return Weapon->GetReloadDelay(bIsShoulder);
}

void AHDCharacterPlayer::ReloadFinished()
{
    Combat->ReloadFinished();

    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK(PlayerController);

    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK(Weapon);
    PlayerController->ChangeCapacityHUDInfo(Weapon->GetCapacityCount());
}

const bool AHDCharacterPlayer::IsShouldering() const
{
	return Combat->IsShoulder();
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    Combat->SetShoulder(bShoulder);   

    if (bShoulder)
    {
        SpringArmArmLengthTimeline.PlayFromStart();
    }
    else
    {
        SpringArmArmLengthTimeline.ReverseFromEnd();
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
    return Combat->GetWeapon();
}

const FVector& AHDCharacterPlayer::GetHitTarget() const
{
    return Combat->GetHitTarget();
}

const EHDCombatState AHDCharacterPlayer::GetCombatState() const
{
    return Combat->GetCombatState();
}

void AHDCharacterPlayer::SetCombatState(const EHDCombatState State)
{
    Combat->SetCombatState(State);
}

void AHDCharacterPlayer::Attack(const bool bActive)
{
    const EHDCombatState CombatState = GetCombatState();
    if (CombatState == EHDCombatState::Unoccupied)
    {
        Fire(bActive);
    }
    else if (CombatState == EHDCombatState::HoldStratagem)
    {
        ThrowStratagem();
    }
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

    SpringArmTargetZOffset             = CharacterControlData->TargetOffset.Z;
    SpringArmTargetArmLength           = CharacterControlData->TargetArmLength;
    SpringArm->TargetArmLength         = CharacterControlData->TargetArmLength;
    SpringArm->TargetOffset            = CharacterControlData->TargetOffset;
    SpringArm->SocketOffset            = CharacterControlData->SocketOffset;
    SpringArm->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
    SpringArm->bInheritPitch           = CharacterControlData->bInheritPitch;
    SpringArm->bInheritYaw             = CharacterControlData->bInheritYaw;
    SpringArm->bInheritRoll            = CharacterControlData->bInheritRoll;
    SpringArm->bDoCollisionTest        = CharacterControlData->bDoCollisionTest;
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

    PrevMovementState = (PrevMovementState != MovementState) ? MovementState : PrevMovementState;
    MovementState = NewState;
	if (bForced)
	{
        SetCombatState(NewState == EHDCharacterMovementState::Prone ? EHDCombatState::Ragdoll : EHDCombatState::Unoccupied);
    }

    ChangeCameraZOffsetByCharacterMovementState(MovementState);
}

void AHDCharacterPlayer::RestoreMovementState()
{
    if (MovementState == EHDCharacterMovementState::Crouch)
    {
        MovementState = EHDCharacterMovementState::Idle;
    }
    else if (MovementState == EHDCharacterMovementState::Prone)
    {
        MovementState = PrevMovementState == EHDCharacterMovementState::Crouch ? PrevMovementState : EHDCharacterMovementState::Idle;
    }
    else
    {
        CONDITION_CHECK(true);
    }

    ChangeCameraZOffsetByCharacterMovementState(MovementState);
}

const FVector& AHDCharacterPlayer::GetThrowDirection() const
{
    return GetHitTarget();
}

void AHDCharacterPlayer::HoldStratagem()
{   
    NULL_CHECK(StratagemClass);

    if (SelectedStratagemName.IsNone() == false)
    {
        SetWeaponActive(false);

        UWorld* World = GetWorld();
        VALID_CHECK(World);

        USkeletalMeshComponent* CharacterMesh = GetMesh();
        const USkeletalMeshSocket* RightHandSocket = CharacterMesh->GetSocketByName(SOCKETNAME_RIGHTHAND);
        NULL_CHECK(RightHandSocket);

        if (GetCombatState() != EHDCombatState::HoldStratagem)
        {
            const FTransform SocketTransform = RightHandSocket->GetSocketTransform(CharacterMesh);
            FRotator TargetRotation = (GetHitTarget() - SocketTransform.GetLocation()).Rotation();
            TargetRotation.Roll = 0;

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Cast<APawn>(this);

            Stratagem = World->SpawnActor<AHDStratagem>(StratagemClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
        }

        NULL_CHECK(Stratagem);

        RightHandSocket->AttachActor(Stratagem, CharacterMesh);
        Stratagem->StratagemName = SelectedStratagemName;
        Stratagem->StratagemActiveDelay = SelecteddStratagemActiveDelay;

        SetCombatState(EHDCombatState::HoldStratagem);
    }

    SelectedStratagemName = FName();
    CommandMatchStratagemNameList.Empty();
    CurrentInputCommandList.Empty();
}

void AHDCharacterPlayer::ThrowFinished()
{
    SetCombatState(EHDCombatState::Unoccupied);
}

void AHDCharacterPlayer::ThrowStratagem()
{
    NULL_CHECK(ThrowMontage);

    // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
    PlayMontage(ThrowMontage);
}

void AHDCharacterPlayer::CancleStratagem()
{
    VALID_CHECK(Stratagem);

    Stratagem->Destroy();

    SetCombatState(EHDCombatState::Unoccupied);
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

    if (IsShouldering())
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
    const bool bIsShoulder = IsShouldering();
    const float TargetFOV = bIsShoulder ? Weapon->GetZoomedFOV() : DefaultFOV;
    const float InterpSpeed = bIsShoulder ? Weapon->GetZoomInterpSpeed() : Combat->GetZoomInterpSpeed();
    const float NewInterpFOV = FMath::FInterpTo(Combat->GetCurrentFOV(), TargetFOV, DeltaSeconds, InterpSpeed);
    Combat->SetCurrentFOV(NewInterpFOV);
    
    FollowCamera->SetFieldOfView(Combat->GetCurrentFOV());
}

void AHDCharacterPlayer::PlayMontage(UAnimMontage* Montage, const FName SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    VALID_CHECK(AnimInstance);
    NULL_CHECK(Montage);

    AnimInstance->Montage_Play(Montage);

    if (SectionName.IsNone() == false)
    {
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void AHDCharacterPlayer::ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterMovementState State)
{
    switch (State)
    {
    case EHDCharacterMovementState::Idle:
        SpringArm->TargetOffset.Z = SpringArmTargetZOffset;
        break;
    case EHDCharacterMovementState::Crouch:
        SpringArm->TargetOffset.Z = SpringArmTargetZOffset - 40.f;
        break;
    case EHDCharacterMovementState::Prone:
        SpringArm->TargetOffset.Z = SpringArmTargetZOffset - 80.f;
        break;
    default:
        LOG("EHDCharacterMovementState is Invalid!!");
        break;
    }
}

void AHDCharacterPlayer::OnCameraSpringArmLengthTimelineUpdate(const float Value)
{
    const float Interpolated = FMath::Lerp(SpringArmTargetArmLength, SpringArmTargetArmLength / 2.f, Value);
    SpringArm->TargetArmLength = Interpolated;
}

const float AHDCharacterPlayer::Fire(const bool IsPressed)
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK_WITH_RETURNTYPE(Weapon, 0.f);

    if(Combat->Fire(IsPressed) == false)
    {
        if (Combat->CanReload() && Weapon->IsAmmoEmpty())
        {
            Reload();
        }

        return 0.f;
    }

    switch (Weapon->GetFireType())
    {
    case EHDFireType::HitScan:
    case EHDFireType::Projectile:
    {
        const FName SectionName = IsShouldering() ? MONTAGESECTIONNAME_RIFLE_AIM : MONTAGESECTIONNAME_RIFLE_HIP;
        PlayMontage(FireWeaponMontage, SectionName);
    }
    break;
    case EHDFireType::Shotgun:
    {
        // TODO
        //const FName SectionName = IsShouldering() ? MONTAGESECTIONNAME_SHOTGUN_AIM : MONTAGESECTIONNAME_SHOTGUN_HIP;
        //PlayMontage(FireWeaponMontage);
        //Shotgun->FireShotgun(TraceHitTargets);
        //CombatState = ECombatState::ECS_Unoccupied;
    }
    break;
    }

    return Weapon->GetFireDelay();
}

const bool AHDCharacterPlayer::FireFinished()
{
    AHDWeapon* Weapon = GetWeapon();
    NULL_CHECK_WITH_RETURNTYPE(Weapon, false);

    AHDPlayerController* PlayerController = GetController<AHDPlayerController>();
    NULL_CHECK_WITH_RETURNTYPE(PlayerController, 0.f);
    PlayerController->ChangeAmmoHUDInfo(Weapon->GetAmmoCount());

    return Combat->FireFinished();
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
