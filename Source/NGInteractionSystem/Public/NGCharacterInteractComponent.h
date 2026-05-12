#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NGCharacterInteractInterface.h"
#include "NGCharacterInteractComponent.generated.h"

class UShapeComponent;
class UCameraComponent;
class INGInteractionInterface;
class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NGINTERACTIONSYSTEM_API UNGCharacterInteractComponent : public UActorComponent, public INGCharacterInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNGCharacterInteractComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// INGCharacterInteractInterface Implementation
	virtual bool TryInteract_Implementation() override;
	virtual UObject* GetCurrentInteractable_Implementation() override;


protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction",
			meta = (AllowedClasses = "/Script/Engine.ShapeComponent,/Script/Engine.BoxComponent,/Script/Engine.CapsuleComponent", ExactClass = false))
	FComponentReference InteractionShapeRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction",
			meta = (AllowedClasses = "/Script/Engine.CameraComponent", ExactClass = false ))
	FComponentReference InteractionCameraRef;

	UShapeComponent* GetInteractionShape() const;
	UCameraComponent* GetInteractionCamera() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TScriptInterface<INGInteractionInterface> CurrentInteractable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractAngleThreshold = 15.0f;

	/** How often (in seconds) to update the current interactable. 0 = every frame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float UpdateInterval = 0.0f;

	UFUNCTION()
	void OnInteractionShapeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionShapeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateCurrentInteractable();
	AActor* GetBestInteractableActor();

	/**
	 * Reconcile ReadyInteractables against OverlappingInteractables by polling
	 * IsReadyToInteract on each candidate. Fires OnEnteredInteractRange when an
	 * overlapping actor transitions to ready, and OnExitedInteractRange (plus
	 * OnDeselectedForInteract first, if it was the current selection) when it
	 * transitions back to not-ready. This is how the overlay material chain
	 * stays in sync with runtime readiness — physical overlap alone is not
	 * enough, since an actor's IsReadyToInteract can flip while it stays in
	 * the zone (e.g. a control panel becoming occupied mid-overlap).
	 */
	void SyncReadyInteractables();

private:
	/** All actors physically overlapping the interaction shape, ready or not. */
	TArray<AActor*> OverlappingInteractables;

	/**
	 * Subset of OverlappingInteractables whose IsReadyToInteract last returned
	 * true. The picker iterates this list, and Enter/Exit events fire when
	 * membership changes.
	 */
	TArray<AActor*> ReadyInteractables;

	float TimeSinceLastUpdate = 0.0f;
};
