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
 * Contract for actors that can be interacted with.
 *
 * The lifecycle splits cleanly along two axes:
 *
 *   Proximity:  OnEnteredInteractRange  <-->  OnExitedInteractRange
 *   Selection:  OnSelectedForInteract   <-->  OnDeselectedForInteract
 *
 * Both can be active at once: a single actor in the interactor's box can be
 * "in range" without being "selected", and the selected one is always also
 * in range. UNGCharacterInteractComponent fires these such that:
 *
 *   - Every actor entering the overlap zone gets OnEnteredInteractRange
 *   - Every actor leaving the overlap zone gets OnExitedInteractRange
 *   - The actor closest to the camera centre (within InteractAngleThreshold)
 *     gets OnSelectedForInteract
 *   - When a different actor becomes the best, the previous best gets
 *     OnDeselectedForInteract (only if still in range — otherwise the
 *     exit event already cleared its visual state)
 *
 * Base class ANGInteractableActor applies InteractReadyMaterial on enter
 * (and again on deselect, reverting from selected), InteractSelectedMaterial
 * on select, and clears the overlay on exit. Subclasses can override any of
 * these independently.
 *
 * IsReadyToInteract is a pure query, consulted by TryInteract at press time
 * to gate the actual Interact call. No side effects in the base
 * implementation — override it to return false for "damaged", "locked", or
 * any other "still selectable, but pressing E does nothing" state.
 */
class NGINTERACTIONSYSTEM_API INGInteractionInterface
{
	GENERATED_BODY()

public:
	/** Pure query — returns whether pressing the interact key should actually fire Interact. No side effects. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool IsReadyToInteract();

	/**
	 * World-space location the look-at picker should aim at when deciding
	 * which overlapping interactable is the "best candidate" — the one the
	 * player is currently looking at.
	 *
	 * Default impl returns GetActorLocation() for backwards compatibility.
	 * Override on actors whose interaction hit-zone is offset from the actor
	 * origin: ANGInteractableActor returns InteractMesh->GetComponentLocation()
	 * so a control panel's interact zone (typically a chair / seat region
	 * offset from the panel mesh's origin) gets aimed at correctly.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FVector GetInteractWorldLocation();

	/** Fired by the interactor's component when this actor enters the proximity zone. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction|Proximity")
	void OnEnteredInteractRange(AActor* Interactor);

	/** Fired when this actor leaves the proximity zone. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction|Proximity")
	void OnExitedInteractRange(AActor* Interactor);

	/** Fired when this actor becomes the centre-most candidate (within InteractAngleThreshold). */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction|Selection")
	void OnSelectedForInteract(AActor* Interactor);

	/** Fired when a different actor takes over as best, and this actor is still in range. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction|Selection")
	void OnDeselectedForInteract(AActor* Interactor);

	/** Fired when the player presses the interact key while this is the selected actor (and IsReadyToInteract returned true). */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* InteractingActor);
};
