// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "GameTwoCharacter.generated.h"

UCLASS(config=Game)
class AGameTwoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AGameTwoCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    virtual void FellOutOfWorld(const class UDamageType & dmgType) override;

    
protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void Turn(float Value);
    
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
    
private:
    UPROPERTY(EditAnywhere, Category = "Movement")
    float RotationFactor = 30.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float BackingFactor = 2.f;
    
    UPROPERTY(EditAnywhere, Category = "Spawning")
    FString SpawnPoint = "Second";

    
    /** Holds the Value when rotate is triggered */
    float RotateValue = 0.f;
    

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
//	/** Returns FollowCamera subobject **/
//	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

