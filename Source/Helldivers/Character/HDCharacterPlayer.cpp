
#include "Character/HDCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HDCharacterControlData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/HDWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "CharacterTypes/HDCharacterControlTypes.h"
#include "Stratagem/HDStratagem.h"
#include "GameData/HDStratagemData.h"
#include "Define/HDDefine.h"
#include "Controller/HDPlayerController.h"
#include "Animation/HDAnimInstance.h"

AHDCharacterPlayer::AHDCharacterPlayer()
    : CurrentCharacterControlType()
    , ArmorType(EHDArmorType::Medium)
    , StartingAimRotation()
    , LastFrame_Yaw(0.f)
    , AimOffset_Yaw(0.f)
    , AimOffset_Pitch(0.f)
    , InterpAimOffset_Yaw(0.f)
    , AimOffsetYawCompensation(0.f)
    , TurnThreshold(90.f)
    , bIsShoulder(false)
    , bIsSprint(false)
    , bUseRotateRootBone(false)
    , TurningInPlace(EHDTurningInPlace::NotTurning)
    , WeaponType(EWeaponType::Count)
    , HitTarget()
    , FireTimer()
    , SelecteddStratagemActiveDelay(0.f)
    , ThrowTimer()
    , DefaultFOV(55.f)
    , ZoomedFOV(30.f)
    , CurrentFOV(0.f)
    , ZoomInterpSpeed(20.f)
    , ErgonomicFactor(0.f)
{
    CombatState = EHDCombatState::Unoccupied;
    PrimaryActorTick.bCanEverTick = true;
    bCanFire = true;
    bIsFireButtonPressed = false; 
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
}

void AHDCharacterPlayer::BeginPlay()
{
    Super::BeginPlay();

    SetCharacterControl(CurrentCharacterControlType);
    SpawnDefaultWeapon();
}

void AHDCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

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

    AimOffset(DeltaTime);

    FHitResult HitResult;
    TraceUnderCrosshairs(HitResult);
    HitTarget = HitResult.ImpactPoint;
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
    UHDCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
    VALID_CHECK(NewCharacterControl);

    SetCharacterControlData(NewCharacterControl);

    APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
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
    CameraBoom->SetRelativeTransform(FTransform(CharacterControlData->RelativeRotation, CharacterControlData->RelativeLocation));
    CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
    CameraBoom->bInheritPitch           = CharacterControlData->bInheritPitch;
    CameraBoom->bInheritYaw             = CharacterControlData->bInheritYaw;
    CameraBoom->bInheritRoll            = CharacterControlData->bInheritRoll;
    CameraBoom->bDoCollisionTest        = CharacterControlData->bDoCollisionTest;
}

void AHDCharacterPlayer::SetArmor(EHDArmorType NewArmorType)
{
    //static ConstructorHelpers::FObjectFinder<UHDCharacterArmorData> ArmorDataAssetRef(TEXT(""));
}

void AHDCharacterPlayer::EquipWeapon(AHDWeapon* NewWeapon)
{
    VALID_CHECK(NewWeapon);

    Weapon = NewWeapon;
    Weapon->SetWeaponState(EWeaponState::Equip);

    const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
    NULL_CHECK(RightHandSocket);

    RightHandSocket->AttachActor(Weapon, GetMesh());
    Weapon->SetOwner(this);
    ErgonomicFactor = Weapon->GetErgonomicFactor();

    AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);

    PlayerController->SetWeaponHUDInfo(Weapon);
}

void AHDCharacterPlayer::PlayFireMontage(const bool bAiming)
{
    VALID_CHECK(Weapon);

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && FireWeaponMontage)
    {
        AnimInstance->Montage_Play(FireWeaponMontage);
        FName SectionName = bIsShoulder ? FName("Rifle_Aim") : FName("Rifle_Hip");
        AnimInstance->Montage_JumpToSection(SectionName);
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

    AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);
    PlayerController->SetHUDActiveByCurrentInputMatchList(CommandMatchStratagemNameList, CurrentInputCommandList.Num());
}

void AHDCharacterPlayer::ThrowStratagem()
{   
    USkeletalMeshComponent* CharacterMesh = GetMesh();
    UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
    if (AnimInstance && ThrowMontage && StratagemClass && SelectedStratagemName.IsNone() == false)
    {
        SetWeaponActive(false);

        // TODO(25/03/27)  추후 Crouch 등 다른 자세 생기면 해당 섹션으로 점프하기
        PlayThrowMontage();

        UWorld* World = GetWorld();
        VALID_CHECK(World);

        const USkeletalMeshSocket* RightHandSocket = CharacterMesh->GetSocketByName(FName("RightHandSocket"));
        NULL_CHECK(RightHandSocket);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = CastChecked<APawn>(this);

        const FTransform SocketTransform = RightHandSocket->GetSocketTransform(CharacterMesh);
        FRotator TargetRotation = (HitTarget - SocketTransform.GetLocation()).Rotation();
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

    FVector Velocity = GetVelocity();
    Velocity.Z = 0.f;
    const float Speed = Velocity.Size();
    const bool bIsFalling = GetCharacterMovement()->IsFalling();
    const FRotator BaseAimRoatation = GetBaseAimRotation();

    if (Speed == 0.f && bIsFalling == false)
    {
        bUseRotateRootBone = false;
        const FRotator CurrentRotation = FRotator(0.f, BaseAimRoatation.Yaw, 0.f);
        const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, StartingAimRotation);
        AimOffset_Yaw = DeltaAimRotation.Yaw;
        if (TurningInPlace == EHDTurningInPlace::NotTurning)
        {
            InterpAimOffset_Yaw = AimOffset_Yaw;
        }
        bUseControllerRotationYaw = true;
        TurnInPlace(DeltaTime);
    }

    if (Speed > 0.f || bIsFalling)
    {
        bUseRotateRootBone = false;
        const FRotator CurrentRotation = FRotator(0.f, BaseAimRoatation.Yaw, 0.f);
        const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, StartingAimRotation);
        AimOffset_Yaw = FMath::Clamp(DeltaAimRotation.Yaw, -90.f, 90.f);
        bUseControllerRotationYaw = false;
        TurningInPlace = EHDTurningInPlace::NotTurning;
        StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
    }

    //const float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.f, BaseAimRoatation.Yaw, 0.f), FRotator(0.f, LastFrame_Yaw, 0.f)).Yaw;
    //AimOffset_Yaw -= DeltaYaw;
    //InterpAimOffset_Yaw -= DeltaYaw;
    //
    //LastFrame_Yaw = BaseAimRoatation.Yaw;

    AimOffset_Pitch = BaseAimRoatation.Pitch;
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
        // Rotate Aim
        InterpAimOffset_Yaw = FMath::FInterpTo(InterpAimOffset_Yaw, 0.f, DeltaTime, ErgonomicFactor / 15.f);
        AimOffset_Yaw = InterpAimOffset_Yaw;
        if (FMath::Abs(AimOffset_Yaw) < 15.f)
        {
            AimOffset_Yaw = 15.f;
            StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
            TurningInPlace = EHDTurningInPlace::NotTurning;
        }
    }
}

void AHDCharacterPlayer::SpawnDefaultWeapon()
{
    NULL_CHECK(DefaultWeaponClass);

    UWorld* World = GetWorld();
    VALID_CHECK(World);
    
    AHDWeapon* SpawnWeapon = World->SpawnActor<AHDWeapon>(DefaultWeaponClass);
    NULL_CHECK(SpawnWeapon);

    EquipWeapon(SpawnWeapon);
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

void AHDCharacterPlayer::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
    FVector2D ViewportSize = FVector2D();
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }

    const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;
    const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection
    );
    
    if (bScreenToWorld)
    {
        FVector Start = CrosshairWorldPosition;
        const float DistanceToCharacter = (GetActorLocation() - Start).Size();
        Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);

        const FVector End = Start + CrosshairWorldDirection * HITSCAN_TRACE_LENGTH;
        UWorld* World = GetWorld();
        VALID_CHECK(World);
        
        World->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
    }
}

void AHDCharacterPlayer::InterpFOV(float DeltaSeconds)
{
    if (IsValid(Weapon) == false)
    {
        return;
    }

    const float TargetFOV = bIsShoulder ? Weapon->GetZoomedFOV() : DefaultFOV;
    const float InterpSpeed = bIsShoulder ? Weapon->GetZoomInterpSpeed() : ZoomInterpSpeed;
    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, InterpSpeed);
    
    VALID_CHECK(FollowCamera);

    FollowCamera->SetFieldOfView(CurrentFOV);
}

const bool AHDCharacterPlayer::CanFire()
{
    if (IsValid(Weapon) == false || bCanFire == false || Weapon->IsAmmoEmpty())
    {
        return false;
    }

    if (WeaponType == EWeaponType::Shotgun && CombatState == EHDCombatState::Reloading)
    {
        return true;
    }

    return (bCanFire && CombatState == EHDCombatState::Unoccupied);
}

void AHDCharacterPlayer::Fire(const bool IsPressed)
{
    bIsFireButtonPressed = IsPressed;

    if (bIsFireButtonPressed == false || CanFire() == false)
    {
        return;
    }

    switch (Weapon->GetFireType())
    {
    case EFireType::HitScan:
    case EFireType::Projectile:
    {
        if (CombatState == EHDCombatState::Unoccupied)
        {
            PlayFireMontage(bIsShoulder);
            Weapon->Fire(HitTarget);

            AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
            NULL_CHECK(PlayerController);
            PlayerController->ChangeAmmoHUDInfo(Weapon->GetAmmoCount());
        }
    }
    break;
    case EFireType::Shotgun:
    {
        if (CombatState == EHDCombatState::Reloading || CombatState == EHDCombatState::Unoccupied)
        {
            // TO DO
            //PlayFireMontage(bIsShoulder);
            //Shotgun->FireShotgun(TraceHitTargets);
            //CombatState = ECombatState::ECS_Unoccupied;
        }
    }
    break;
    }

    StartFireTimer();
}

void AHDCharacterPlayer::StartFireTimer()
{
    if (IsValid(Weapon) == false)
    {
        return;
    }

    GetWorldTimerManager().SetTimer(FireTimer, this, &AHDCharacterPlayer::FireTimerFinished, Weapon->GetFireDelay());
}

void AHDCharacterPlayer::FireTimerFinished()
{
    if (IsValid(Weapon) == false)
    {
        return;
    }

    bCanFire = true;
    if(bIsFireButtonPressed && Weapon->IsAutoFire())
    {
        Fire(true);
    }
    
    // TODO Reload
   /* AHDPlayerController* PlayerController = Cast<AHDPlayerController>(GetController());
    NULL_CHECK(PlayerController);
    PlayerController->ChangeCapacityHUDInfo(Weapon->GetCapacityCount());*/
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
    if (PlayerController)
    {
        DisableInput(PlayerController);
    }
}
