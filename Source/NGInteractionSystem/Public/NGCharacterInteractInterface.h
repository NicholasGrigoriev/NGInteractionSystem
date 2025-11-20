#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NGCharacterInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNGCharacterInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for characters or components that can perform interactions.
 */
class NGINTERACTIONSYSTEM_API INGCharacterInteractInterface
{
	GENERATED_BODY()

public:
	/** Attempt to interact with the current target. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool TryInteract();

	/** Get the current interactable object being focused/selected. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	UObject* GetCurrentInteractable();
};
