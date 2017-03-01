// Fill out your copyright notice in the Description page of Project Settings.

#include "GameTwo.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GrabberComponent.h"


// Sets default values for this component's properties
UGrabberComponent::UGrabberComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    
    // ...
}


// Called when the game starts
void UGrabberComponent::BeginPlay()
{
    Super::BeginPlay();
    
    FindPhysicsHandleComponent();
    FindInputComponent();
}


// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    FVector PlayerLocation;
    FRotator PlayerRotation;
    
    PlayerLocation = GetOwner()->GetActorLocation();
    
    FVector GrabEnd = GetOwner()->GetActorForwardVector();
    GrabEnd *= Reach;
    GrabEnd += PlayerLocation;
    
    //    UE_LOG(LogTemp, Warning, TEXT("Position %s, GrabEnd %s"), *PlayerLocation.ToString(), *GrabEnd.ToString())
    
    DrawDebugLine(GetWorld(), PlayerLocation, GrabEnd, FColor(255 ,0, 0), false, 0.f, 0.f, 5.f);
    
    if(PhysicsHandle->GrabbedComponent)
    {
        FVector GrabLocation = GetReachLineEnd();
        if (Rised)
        {
            GrabLocation.Z += 100.f;
        }
            
        PhysicsHandle->SetTargetLocation(GrabLocation);
    }
}

void UGrabberComponent::FindPhysicsHandleComponent()
{
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
    if (!PhysicsHandle)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing PhysicsHandle component"), *GetOwner()->GetName())
    }
}

void UGrabberComponent::FindInputComponent()
{
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
    if (InputComponent)
    {
        InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabberComponent::GrabPressed);
        InputComponent->BindAction("Grab", IE_Released, this, &UGrabberComponent::GrabReleased);
        InputComponent->BindAction("Rise", IE_Pressed, this, &UGrabberComponent::RisePressed);
        InputComponent->BindAction("Rise", IE_Released, this, &UGrabberComponent::RiseReleased);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing InputComponent"), *GetOwner()->GetName())
    }
}

const FHitResult UGrabberComponent::GetFirstPhysicsInReach()
{
    // LineTrace out to reach distance:
    
    /// Setup query parameters
    FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
    
    FHitResult HitResult;
    GetWorld()->LineTraceSingleByObjectType(HitResult,
                                            GetOwner()->GetActorLocation(),
                                            GetReachLineEnd(),
                                            FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), //Hva skal vi teste mot
                                            TraceParameters
                                            );
    
    return HitResult;
}

void UGrabberComponent::GrabPressed()
{
    FHitResult HitResult = GetFirstPhysicsInReach();
    UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();    //The mesh
    AActor* ActorHit = HitResult.GetActor();
    
    if(ActorHit)
    {
        //GrabComponent deprecated
        PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab,
                                                           NAME_None,  // bone to use - none here
                                                           ComponentToGrab->GetOwner()->GetActorLocation(),
                                                           ComponentToGrab->GetOwner()->GetActorRotation()
                                                           );
    }
}


void UGrabberComponent::GrabReleased()
{
    PhysicsHandle->ReleaseComponent();
}

void UGrabberComponent::RisePressed()
{
    Rised = true;
}

void UGrabberComponent::RiseReleased()
{
    GrabReleased();
    Rised = false;
}

FVector UGrabberComponent::GetReachLineEnd()
{
    FVector GrabEnd = GetOwner()->GetActorForwardVector();
    GrabEnd *= Reach;
    GrabEnd += GetOwner()->GetActorLocation();
    
    return GrabEnd;
}


