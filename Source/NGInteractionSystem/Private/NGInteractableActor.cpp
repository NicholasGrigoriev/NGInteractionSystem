#include "NGInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"

// Sets default values
ANGInteractableActor::ANGInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractMesh"));
	RootComponent = InteractMesh;

	bIsInteractable = true;
}

void ANGInteractableActor::Interact_Implementation(AActor* InteractingActor)
{
	if (!bIsInteractable) return;

	// Base implementation can be empty or log something
	UE_LOG(LogTemp, Log, TEXT("Interacted with %s"), *GetName());
}

bool ANGInteractableActor::IsReadyToInteract_Implementation()
{
	return bIsInteractable;
}

void ANGInteractableActor::ReadyToInteract_Implementation()
{
	if (InteractMesh && InteractReadyMaterial)
	{
		InteractMesh->SetOverlayMaterial(InteractReadyMaterial);
	}
}

void ANGInteractableActor::SelectedToInteract_Implementation()
{
	if (InteractMesh && InteractSelectedMaterial)
	{
		InteractMesh->SetOverlayMaterial(InteractSelectedMaterial);
	}
}

void ANGInteractableActor::NotReadyToInteract_Implementation()
{
	if (InteractMesh)
	{
		InteractMesh->SetOverlayMaterial(nullptr);
	}
}
