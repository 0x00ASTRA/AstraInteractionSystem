// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "InteractableInterface.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bUseCameraForwardVector = true;
	TraceDistance = 300.f;
	TraceRadius = 10.f;
	TraceDimensions = FVector(TraceRadius, TraceRadius, TraceDistance);
	TraceStartOffset = FVector(0, 0, 0);
	TraceEndOffset = FVector(0, 0, 0);
	TraceDuration = 2.f;
	bShowTrace = false;
	CollisionChannel = ECollisionChannel::ECC_Visibility;
    bPersistTraceShape = false;
    TraceShape = ETraceShape::Line;
    HitColor = FColor::Green;
    NoHitColor = FColor::White;
}


bool UInteractionComponent::ShouldTrace() const
{
    return GetWorld() != nullptr && GetOwner() != nullptr && TraceDistance > 0.f;
}

void UInteractionComponent::Interact() const
{
    if (FHitResult const Hit = PerformTrace(); Hit.bBlockingHit)
    {
        if (AActor* HitActor= Hit.GetActor())
        {
            if (HitActor->Implements<UInteractableInterface>())
            {
                IInteractableInterface::Execute_InteractFrom(HitActor, GetOwner());
            }
            if (OnInteract.IsBound())
            {
                OnInteract.Broadcast(HitActor);
            }
        }
    }
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

FHitResult UInteractionComponent::PerformTrace() const
{
    if (!ShouldTrace())
    {
        return FHitResult();
    }

    FHitResult HitRes;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    Params.bTraceComplex = true;

    FVector Loc;
    FRotator Rot;
    if (bUseCameraForwardVector)
    {
        GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Loc, Rot);
    }
    else
    {
        Loc = GetOwner()->GetActorLocation();
        Rot = GetOwner()->GetActorRotation();
    }
    const FVector Start = Loc + TraceStartOffset;
    const FVector End = Loc + (Rot.Vector() * TraceDistance) + TraceEndOffset;
    bool bHit = false;

    switch (TraceShape)
    {
    case ETraceShape::Line:
        bHit = GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, CollisionChannel, Params);
        break;
    case ETraceShape::Sphere:
        bHit = GetWorld()->SweepSingleByChannel(HitRes, Start, End, Rot.Quaternion(), CollisionChannel, FCollisionShape::MakeSphere(TraceRadius), Params);
        break;
    case ETraceShape::Box:
        bHit = GetWorld()->SweepSingleByChannel(HitRes, Start, End, Rot.Quaternion(), CollisionChannel, FCollisionShape::MakeBox(TraceDimensions / 2), Params);
        break;
    }

    if (bShowTrace)
    {
        // Use hit location if a collision occurred; otherwise use the full trace End.
        const FVector DebugEnd = bHit ? HitRes.Location : End;
        const FVector TraceDirection = (DebugEnd - Start).GetSafeNormal();

        switch (TraceShape)
        {
        case ETraceShape::Line:
        {
            if (bHit)
            {
                DrawDebugLine(GetWorld(), Start, HitRes.Location, HitColor, bPersistTraceShape, TraceDuration);
                DrawDebugPoint(GetWorld(), HitRes.Location, 15.f, HitColor, bPersistTraceShape, TraceDuration);
            }
            else
            {
                DrawDebugLine(GetWorld(), Start, End, NoHitColor, false, TraceDuration);
            }
        }
        break;
        case ETraceShape::Sphere:
        {
            // Represent the swept sphere as a capsule.
            const float CapsuleHalfHeight = (DebugEnd - Start).Size() * 0.5f;
            const FVector CapsuleCenter = (Start + DebugEnd) * 0.5f;
            // Align the capsule's long axis (Z) with the trace direction.
            const FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(TraceDirection).ToQuat();

            if (bHit)
            {
                DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, TraceRadius, CapsuleRotation, HitColor, bPersistTraceShape, TraceDuration);
            }
            else
            {
                DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, TraceRadius, CapsuleRotation, NoHitColor, false, TraceDuration);
            }
        }
        break;
        case ETraceShape::Box:
        {
            const FVector BoxCenter = (Start + DebugEnd) * 0.5f;
            const FQuat BoxRotation = FRotationMatrix::MakeFromZ(TraceDirection).ToQuat();

            if (bHit)
            {
                DrawDebugBox(GetWorld(), BoxCenter, TraceDimensions / 2, BoxRotation, HitColor, bPersistTraceShape, TraceDuration);
            }
            else
            {
                DrawDebugBox(GetWorld(), BoxCenter, TraceDimensions / 2, BoxRotation, NoHitColor, false, TraceDuration);
            }
        }
        break;
        }
    }
    return HitRes;
}
