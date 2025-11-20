#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NGInteractionInterface.h"
#include "NGInteractableActor.generated.h"

class UMaterialInstance;
class UStaticMeshComponent;

UCLASS()
class NGINTERACTIONSYSTEM_API ANGInteractableActor : public AActor, public INGInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANGInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// INGInteractionInterface Implementation
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual void ReadyToInteract_Implementation() override;
	virtual void SelectedToInteract_Implementation() override;
	virtual void NotReadyToInteract_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> InteractMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Visuals")
	TObjectPtr<UMaterialInstance> InteractReadyMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Visuals")
	TObjectPtr<UMaterialInstance> InteractSelectedMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bIsInteractable;
};
