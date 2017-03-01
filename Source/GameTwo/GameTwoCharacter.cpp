// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GameTwo.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "GameTwoCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AGameTwoCharacter

AGameTwoCharacter::AGameTwoCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
    // Don't rotate when the controller rotates. Let that just affect the camera.
    //	bUseControllerRotationPitch = false;
    //	bUseControllerRotationYaw = false;
    //	bUseControllerRotationRoll = false;
    
    // Configure character movement
    //    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
    //    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    //    GetCharacterMovement()->JumpZVelocity = 600.f;
    //    GetCharacterMovement()->AirControl = 0.2f;
    
    //    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character
    CameraBoom-> bUseControllerViewRotation = false; // Rotate the arm based on the controller
    CameraBoom->SetWorldRotation(FVector(0.f, -55.f, 0.f).Rotation());
    //
    //    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the
}

void AGameTwoCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (RotateValue > 0.1f || RotateValue < -0.1f)
    {
        AddControllerYawInput(RotateValue * RotationFactor * DeltaTime);
    }
}

void AGameTwoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AGameTwoCharacter::MoveForward);
    PlayerInputComponent->BindAxis("Turn", this, &AGameTwoCharacter::MoveRight);
}

void AGameTwoCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        if (Value < 0.f)
        {
            Value /= BackingFactor;
        }
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AGameTwoCharacter::MoveRight(float Value)
{
    RotateValue = Value;
}
