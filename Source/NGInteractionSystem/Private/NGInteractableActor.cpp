#include "NGInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "GameFramework/Actor.h"

ANGInteractableActor::ANGInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractMesh"));
	RootComponent = InteractMesh;

	bIsInteractable = true;
}

bool ANGInteractableActor::IsReadyToInteract_Implementation()
{
	// Pure query — no side effects. Override to return false when the actor
	// is in a state that should refuse interaction (damaged, locked, busy).
	return bIsInteractable;
}

FVector ANGInteractableActor::GetInteractWorldLocation_Implementation()
{
	// Use the InteractMesh's world position so the look-at picker tracks the
	// actual hit-zone, even when InteractMesh has been re-parented to a non-
	// root component (e.g. positioned to cover only a chair on a console).
	if (InteractMesh)
	{
		return InteractMesh->GetComponentLocation();
	}
	return GetActorLocation();
}

void ANGInteractableActor::OnEnteredInteractRange_Implementation(AActor* /*Interactor*/)
{
	if (InteractMesh && InteractReadyMaterial)
	{
		InteractMesh->SetOverlayMaterial(InteractReadyMaterial);
	}
}

void ANGInteractableActor::OnExitedInteractRange_Implementation(AActor* /*Interactor*/)
{
	if (InteractMesh)
	{
		InteractMesh->SetOverlayMaterial(nullptr);
	}
}

void ANGInteractableActor::OnSelectedForInteract_Implementation(AActor* /*Interactor*/)
{
	if (InteractMesh && InteractSelectedMaterial)
	{
		InteractMesh->SetOverlayMaterial(InteractSelectedMaterial);
	}
}

void ANGInteractableActor::OnDeselectedForInteract_Implementation(AActor* /*Interactor*/)
{
	// Revert to the "in range but not the focus" appearance.
	if (InteractMesh && InteractReadyMaterial)
	{
		InteractMesh->SetOverlayMaterial(InteractReadyMaterial);
	}
}

void ANGInteractableActor::Interact_Implementation(AActor* /*InteractingActor*/)
{
	// Base implementation is empty — override to do something interesting.
}
