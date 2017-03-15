// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GameTwo.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "GameTwoGameMode.h"
#include "MySaveGame.h"
#include "GameTwoCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AGameTwoCharacter

AGameTwoCharacter::AGameTwoCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
    // I want the characters yaw to be the same as the controller:
    bUseControllerRotationYaw = true;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
    
    
    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character
    //Set the default rotation of the arm
    CameraBoom->SetWorldRotation(FVector(0.f, -55.f, 0.f).Rotation());
    
    // Create a camera and place it on the arm
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the
}

void AGameTwoCharacter::BeginPlay()
{
    Super::BeginPlay();
    if(GetWorld())
    {
        //This is just so I have the mouse cursor available in the editor:
        //I don't use it in the game
        GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
        
        
        //Get the current GameMode, casting it to our own GameMode
        //Needed in the following trick.
        auto CurrentGameMode = Cast<AGameTwoGameMode>(GetWorld()->GetAuthGameMode());
        
        //Force the player to spawn at the "First" PlayerStart.
        if (Controller)
        {
            //Get the PlayerStart with the wanted tag:
            AActor *NewPawn = CurrentGameMode->FindPlayerStart(Controller, "First");
            SetActorLocation(NewPawn->GetActorLocation());
            Controller->ClientSetRotation(NewPawn->GetActorRotation());
        }
    }
    LoadGame();
}

void AGameTwoCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (RotateValue > 0.1f || RotateValue < -0.1f)
    {
        AddControllerYawInput(RotateValue * RotationFactor * DeltaTime);
    }
}

void AGameTwoCharacter::FellOutOfWorld(const UDamageType & dmgType)
{
    //If we call the super version, the character gets destroyed
    //I don't want that
    //Super::FellOutOfWorld(dmgType);
    
    UE_LOG(LogTemp, Warning, TEXT("Fell Down. I'm dead..."))
    if(GetWorld()) {
        //Get the current GameMode, casting it to our own GameMode
        auto CurrentGameMode = Cast<AGameTwoGameMode>(GetWorld()->GetAuthGameMode());
        
        //Get the PlayerStart with the wanted tag:
        AActor *NewPawn = CurrentGameMode->FindPlayerStart(Controller, SpawnPoint);
        //Set the actors location to this:
        SetActorLocation(NewPawn->GetActorLocation());
        //The rotation of the character in this game is controlled by the controller rotation
        //so we set the controller rotation
        Controller->ClientSetRotation(NewPawn->GetActorRotation());
        
        
        //Failed attempts:
        
        //Not needed, because we did not do anything with the player
        //It is still alive
        //CurrentGameMode->RestartPlayer(Controller);
        //Did not work the expected way...
        //TempGameMode->RespawnPlayer(Controller);
    }
}

void AGameTwoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AGameTwoCharacter::Interact);
    
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AGameTwoCharacter::MoveForward);
    PlayerInputComponent->BindAxis("Turn", this, &AGameTwoCharacter::Turn);
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

void AGameTwoCharacter::Turn(float Value)
{
    RotateValue = Value;
}

void AGameTwoCharacter::Interact()
{
    Ammo += 10;
    SaveGame();
    
}

void AGameTwoCharacter::SaveGame()
{
    UMySaveGame* SavedGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
    
    SavedGame->Ammo = this->Ammo;
    
    UGameplayStatics::SaveGameToSlot(SavedGame, TEXT("LevelChange"), 0);
    
    UE_LOG(LogTemp, Warning, TEXT("Saved Ammo: %d"), Ammo)
}

void AGameTwoCharacter::LoadGame()
{
    
    UMySaveGame* SavedGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
    
    SavedGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("LevelChange"), 0));
    
    if (SavedGame)
    {
        this->Ammo = SavedGame->Ammo;
        UE_LOG(LogTemp, Warning, TEXT("Loaded Ammo: %d"), Ammo)
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Loading failed"))
    }
    
    
}
