// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GAMETWO_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
    UMySaveGame();

    UPROPERTY(EditAnywhere)
    int Ammo;
};
