
#include "Character/HDCharacterPlayer.h"
#include "Define/HDDefine.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterTypes/HDCharacterStateTypes.h"
#include "Component/Character/HDCombatComponent.h"
#include "HDCharacterControlData.h"
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
    : CurrentCharacterControlType()
    , StartingAimRotation()
    , LastFrame_Yaw(0.f)
    , AimOffset_Yaw(0.f)
    , InterpAimOffset_Yaw(0.f)
    , AimOffset_Pitch(0.f)
    , AimOffsetYawCompensation(0.f)
    , bIsSprint(false)
    , bIsCharacterLookingViewport(false)
    , bUseRotateRootBone(false)
    , TurnThreshold(90.f)
    , TurningInPlace(EHDTurningInPlace::NotTurning)
    , SelecteddStratagemActiveDelay(0.f)
    , DefaultFOV(55.f)
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    // Camera
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Combat
    Combat = CreateDefaultSubobject<UHDCombatComponent>(TEXT("Combat"));
    Combat->CombatState = EHDCombatState::Unoccupied;

    // Input
    static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Helldivers/Input/Action/IA_ChangeControl.IA_ChangeControl'"));
    if (InputChangeActionControlRef.Succeeded())
    {
        ChangeControlAction = InputChangeActionControlRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionThirdPersonLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Helldivers/Input/Action/IA_ThirdPersonLook.IA_ThirdPersonLook'"));
    if (InputActionThirdPersonLookRef.Succeeded())
    {
        ThirdPersonLookAction = InputActionThirdPersonLookRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionThirdPersonMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Helldivers/Input/Action/IA_ThirdPersonMove.IA_ThirdPersonMove'"));
    if (InputActionThirdPersonMoveRef.Succeeded())
    {
        ThirdPersonMoveAction = InputActionThirdPersonMoveRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFirstPersonLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Helldivers/Input/Action/IA_FirstPerconLook.IA_FirstPerconLook'"));
    if (InputActionFirstPersonLookRef.Succeeded())
    {
        FirstPersonLookAction = InputActionFirstPersonLookRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFirstPersonMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Helldivers/Input/Action/IA_FirstPerconMove.IA_FirstPerconMove'"));
    if (InputActionFirstPersonMoveRef.Succeeded())
    {
        FirstPersonMoveAction = InputActionFirstPersonMoveRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> StratagemDataListRef(TEXT("/Script/Engine.DataTable'/Game/Helldivers/GameData/DT_StratagenData.DT_StratagenData'"));
    if (StratagemDataListRef.Succeeded())
    {
        AvaliableStratagemDataTable = StratagemDataListRef.Object;
    }

    CurrentCharacterControlType = EHDCharacterControlType::ThirdPerson;
    bUseControllerRotationYaw = false;

    GetCapsuleComponent()->Activate(false);
}

void AHDCharacterPlayer::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    SpawnDefaultWeapon();
}

void AHDCharacterPlayer::BeginPlay()
{
    Super::BeginPlay();

    AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);
    DisableInput(PlayerController);

    NULL_CHECK(DefaultCurve);

    FOnTimelineFloat ArmLengthTimelineProgress;
    ArmLengthTimelineProgress.BindUFunction(this, FName("OnArmLengthTimelineUpdate"));
    ArmLengthTimeline.AddInterpFloat(DefaultCurve, ArmLengthTimelineProgress);
    ArmLengthTimeline.SetLooping(false);

    FOnTimelineFloat TuringTimelineProgress;
    TuringTimelineProgress.BindUFunction(this, FName("OnTurningTimelineUpdate"));
    TurningTimeline.AddInterpFloat(DefaultCurve, TuringTimelineProgress);
    TurningTimeline.SetLooping(false);
}

void AHDCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    SetCharacterControl(CurrentCharacterControlType);

    AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);

    PlayerController->PlayerCameraManager->ViewPitchMin = -60.f;
    PlayerController->PlayerCameraManager->ViewPitchMax = 70.f;
    PlayerController->SetWeaponHUDInfo(Weapon);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    VALID_CHECK(EnhancedInputComponent);
    EnhancedInputComponent->BindAction(ThirdPersonMoveAction, ETriggerEvent::Triggered, this, &AHDCharacterPlayer::ThirdPersonMove);
    EnhancedInputComponent->BindAction(ThirdPersonLookAction, ETriggerEvent::Triggered, this, &AHDCharacterPlayer::ThirdPersonLook);
    EnhancedInputComponent->BindAction(FirstPersonMoveAction, ETriggerEvent::Triggered, this, &AHDCharacterPlayer::FirstPersonMove);
    EnhancedInputComponent->BindAction(FirstPersonLookAction, ETriggerEvent::Triggered, this, &AHDCharacterPlayer::FirstPersonLook);
    EnhancedInputComponent->BindAction(ChangeControlAction,   ETriggerEvent::Triggered, this, &AHDCharacterPlayer::ChangeCharacterControlType);
}

void AHDCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ArmLengthTimeline.TickTimeline(DeltaTime);
    TurningTimeline.TickTimeline(DeltaTime);
    AimOffset(DeltaTime);        
}

void AHDCharacterPlayer::ChangeCharacterControlType()
{
    if (CurrentCharacterControlType == EHDCharacterControlType::FirstPerson)
    {
        SetCharacterControl(EHDCharacterControlType::ThirdPerson);
    }
    else if (CurrentCharacterControlType == EHDCharacterControlType::ThirdPerson)
    {
        SetCharacterControl(EHDCharacterControlType::FirstPerson);
    }
}

void AHDCharacterPlayer::SetCharacterControl(const EHDCharacterControlType NewCharacterControlType)
{
    UHDCharacterControlData* NewCharacterControl = CharacterControlDataMap[NewCharacterControlType];
    VALID_CHECK(NewCharacterControl);

    SetCharacterControlData(NewCharacterControl);

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    NULL_CHECK(PlayerController);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    NULL_CHECK(Subsystem);

    Subsystem->ClearAllMappings();
    UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
    if (NewMappingContext)
    {
        Subsystem->AddMappingContext(NewMappingContext, 0);
    }

    CurrentCharacterControlType = NewCharacterControlType;
}

void AHDCharacterPlayer::OnArmLengthTimelineUpdate(const float Value)
{
    NULL_CHECK(CameraBoom);

    const float DefaultArmLength =  CharacterControlDataMap[CurrentCharacterControlType]->TargetArmLength;
    const float Interpolated = FMath::Lerp(DefaultArmLength, DefaultArmLength / 2.f, Value);
    CameraBoom->TargetArmLength = Interpolated;
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

void AHDCharacterPlayer::SetCharacterControlData(UHDCharacterControlData* CharacterControlData)
{
    Super::SetCharacterControlData(CharacterControlData);

    CameraBoom->TargetArmLength         = CharacterControlData->TargetArmLength;
    CameraBoom->TargetOffset            = CharacterControlData->TargetOffset;
    CameraBoom->SocketOffset            = CharacterControlData->SocketOffset;
    CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
    CameraBoom->bInheritPitch           = CharacterControlData->bInheritPitch;
    CameraBoom->bInheritYaw             = CharacterControlData->bInheritYaw;
    CameraBoom->bInheritRoll            = CharacterControlData->bInheritRoll;
    CameraBoom->bDoCollisionTest        = CharacterControlData->bDoCollisionTest;
}

const bool AHDCharacterPlayer::IsShouldering() const
{
    NULL_CHECK_WITH_RETURNTYPE(Combat, false);
    return Combat->bIsShoulder;
}

void AHDCharacterPlayer::SetShouldering(const bool bShoulder)
{
    NULL_CHECK(Combat);
    Combat->bIsShoulder = bShoulder;
    if(bShoulder)
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
    NULL_CHECK_WITH_RETURNTYPE(Combat, FVector::ZeroVector);
    return Combat->HitTarget;
}

const EHDCombatState AHDCharacterPlayer::GetCombatState() const
{
    NULL_CHECK_WITH_RETURNTYPE(Weapon, EHDCombatState::Count);
    return Combat->CombatState;
}

void AHDCharacterPlayer::PlayFireMontage(const bool bAiming)
{
    VALID_CHECK(Weapon);
    NULL_CHECK(Combat);

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

const FVector& AHDCharacterPlayer::GetThrowDirection() const
{
    NULL_CHECK_WITH_RETURNTYPE(Combat, FVector::ZeroVector);
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
    NULL_CHECK(Combat);

    if (IsValid(Weapon) == false)
    {
        return;
    }

    UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
    NULL_CHECK(CharacterMovementComponent);

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

void AHDCharacterPlayer::OnTurningTimelineUpdate(const float Value)
{

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

void AHDCharacterPlayer::ThirdPersonMove(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling())
    {
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator Rotation = Controller->GetControlRotation();

    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void AHDCharacterPlayer::ThirdPersonLook(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void AHDCharacterPlayer::FirstPersonMove(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling())
    {
        return;
    }

    const FVector2D& MovementVector = Value.Get<FVector2D>();

    const FRotator& Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector& ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector& RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void AHDCharacterPlayer::FirstPersonLook(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(-LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
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
    
    VALID_CHECK(FollowCamera);

    FollowCamera->SetFieldOfView(Combat->CurrentFOV);
}

void AHDCharacterPlayer::Fire(const bool IsPressed)
{
    NULL_CHECK(Combat);

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
