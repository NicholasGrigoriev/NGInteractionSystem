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

private:
	TArray<AActor*> OverlappingInteractables;
	float TimeSinceLastUpdate = 0.0f;
};
