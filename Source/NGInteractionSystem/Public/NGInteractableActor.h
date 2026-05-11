#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NGInteractionInterface.h"
#include "NGInteractableActor.generated.h"

class UMaterialInstance;
class UStaticMeshComponent;

/**
 * Convenience base class for interactable actors. Provides:
 *   - A StaticMesh root
 *   - Two overlay-material slots (ready + selected) driven by the interaction
 *     lifecycle events
 *   - A simple bIsInteractable bool gating IsReadyToInteract
 *
 * Subclasses can override any single lifecycle method without touching the
 * others — e.g. an override of OnSelectedForInteract that plays an audio
 * cue and DOESN'T set the overlay material, leaving the "ready" highlight
 * visible even while selected.
 */
UCLASS()
class NGINTERACTIONSYSTEM_API ANGInteractableActor : public AActor, public INGInteractionInterface
{
	GENERATED_BODY()

public:
	ANGInteractableActor();

	// INGInteractionInterface
	virtual bool IsReadyToInteract_Implementation() override;
	virtual void OnEnteredInteractRange_Implementation(AActor* Interactor) override;
	virtual void OnExitedInteractRange_Implementation(AActor* Interactor) override;
	virtual void OnSelectedForInteract_Implementation(AActor* Interactor) override;
	virtual void OnDeselectedForInteract_Implementation(AActor* Interactor) override;
	virtual void Interact_Implementation(AActor* InteractingActor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> InteractMesh;

	/** Applied on OnEnteredInteractRange and re-applied on OnDeselectedForInteract. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Visuals")
	TObjectPtr<UMaterialInstance> InteractReadyMaterial;

	/** Applied on OnSelectedForInteract — overlays / replaces the ready material. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Visuals")
	TObjectPtr<UMaterialInstance> InteractSelectedMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bIsInteractable;
};
