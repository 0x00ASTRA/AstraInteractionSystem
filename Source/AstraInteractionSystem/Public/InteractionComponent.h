// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, AActor*, Target);

UENUM(BlueprintType)
enum class ETraceShape : uint8
{
	Line,
	Sphere,
	Box
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTRAINTERACTIONSYSTEM_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	
UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="The maximum distance the trace will reach. Starts from the camera location if bUseCameraForwardVector is checked, uses the actor's location otherwise. Computed as: StartLocation + (ForwardVector * TraceDistance)."))
    float TraceDistance;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="The radius of the sphere collision shape. Only used when TraceShape is set to Sphere. Ignored otherwise."))
    float TraceRadius;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="The dimensions (X, Y, Z) of the box collision shape. Only used when TraceShape is set to Box. Ignored otherwise."))
    FVector TraceDimensions;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="The collision channel the trace will use. Custom channels can be defined in Project Settings under Collision. Determines what objects the trace can hit."))
    TEnumAsByte<ECollisionChannel> CollisionChannel;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="If true, the trace aligns with the camera's forward vector, starting from the camera's viewpoint and extending in its facing direction. Typically used in first-person games. If false, uses the actor's location and rotation instead."))
    bool bUseCameraForwardVector;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="Offset applied to the trace's starting point, relative to the camera or actor location (depending on bUseCameraForwardVector). Useful for adjusting the trace origin."))
    FVector TraceStartOffset;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="Offset applied to the trace's endpoint, added after calculating the trace distance. Adjusts the final position of the trace."))
    FVector TraceEndOffset;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="If true, displays debug visualization for the trace (line, sphere, or box) in the viewport, using HitColor for hits and NoHitColor for misses."))
    bool bShowTrace;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="Duration (in seconds) that the debug trace visualization remains visible when bShowTrace is true. Ignored if bPersistTraceShape is true."))
    float TraceDuration;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="The shape of the trace: Line (single ray), Sphere (swept sphere), or Box (swept box). Determines the collision method and debug visualization."))
    ETraceShape TraceShape;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="Color of the debug visualization when the trace hits an object. Only visible if bShowTrace is true."))
    FColor HitColor;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="Color of the debug visualization when the trace does not hit an object. Only visible if bShowTrace is true."))
    FColor NoHitColor;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ToolTip="If true, the debug trace visualization persists until manually cleared or the actor is destroyed. If false, it disappears after TraceDuration seconds. Requires bShowTrace to be true."))
    bool bPersistTraceShape;

    UFUNCTION(BlueprintCallable, BlueprintPure, meta=(ToolTip="Checks if the trace can be performed. Returns true if the world and owner exist and TraceDistance is positive, false otherwise."))
    bool ShouldTrace() const;

    UFUNCTION(BlueprintCallable, meta=(ToolTip="Performs a trace and triggers interaction if it hits an object implementing the Interactable interface. Broadcasts the OnInteract delegate with the hit actor as the target. Does not modify the component's state."))
    void Interact() const;

    UPROPERTY(BlueprintAssignable, meta=(ToolTip="Delegate triggered when an interaction occurs, passing the hit actor as the target. Bind to this in Blueprints to respond to successful interactions."))
    FOnInteract OnInteract;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FHitResult PerformTrace() const;
};
