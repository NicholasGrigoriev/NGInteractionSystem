#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NGInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNGInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be interacted with.
 */
class NGINTERACTIONSYSTEM_API INGInteractionInterface
{
	GENERATED_BODY()

public:
	/** Called when the actor is interacted with. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* InteractingActor);

	/** Called when the actor is looked at/ready to be interacted with. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void ReadyToInteract();

	/** Called when the actor is selected for interaction (e.g. mouse hover or specific selection). */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void SelectedToInteract();

	/** Called when the actor is no longer ready to be interacted with. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void NotReadyToInteract();
};
