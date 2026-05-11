#include "NGInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"

// Sets default values
ANGInteractableActor::ANGInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractMesh"));
	RootComponent = InteractMesh;

	bIsInteractable = true;
}



void ANGInteractableActor::Interact_Implementation(AActor* InteractingActor)
{
	if (!bIsInteractable) return;

	// Base implementation can be empty or log something
}

bool ANGInteractableActor::IsReadyToInteract_Implementation()
{
	// Side-effect: apply the "ready" overlay material when the proximity system
	// notifies us we're the current candidate. SelectedToInteract is called
	// right after by the component and will overwrite this with the stronger
	// "selected" material, so InteractReadyMaterial is mostly useful for
	// subclasses that override SelectedToInteract to do something else.
	if (InteractMesh && InteractReadyMaterial)
	{
		InteractMesh->SetOverlayMaterial(InteractReadyMaterial);
	}
	return bIsInteractable;
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
