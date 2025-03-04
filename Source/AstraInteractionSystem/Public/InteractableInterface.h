// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionComponent.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};


class ASTRAINTERACTIONSYSTEM_API IInteractableInterface
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintImplementableEvent, Category="Interactable")
	void InteractFrom(AActor* Instigator);
	bool CanInteract() const
	{
		const AActor* Owner = Cast<AActor>(GetOwner());
		if (!Owner)
		{
			return false;
		}
		// Check if the owner has a UInteractionComponent
		return Owner->GetComponentByClass(UInteractionComponent::StaticClass()) != nullptr;
	}

protected:
	UObject* GetOwner() const { return this->_getUObject(); }
};
