#include "NGCharacterInteractComponent.h"
#include "Components/ShapeComponent.h"
#include "Camera/CameraComponent.h"
#include "NGInteractionInterface.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UNGCharacterInteractComponent::UNGCharacterInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UNGCharacterInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	UShapeComponent* InteractionShapeComp = GetInteractionShape();


	if (InteractionShapeComp)
	{
		InteractionShapeComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &UNGCharacterInteractComponent::OnInteractionShapeBeginOverlap);
		InteractionShapeComp->OnComponentEndOverlap.AddUniqueDynamic(this, &UNGCharacterInteractComponent::OnInteractionShapeEndOverlap);
	}
}




// Called every frame
void UNGCharacterInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UpdateInterval > 0.0f)
	{
		TimeSinceLastUpdate += DeltaTime;
		if (TimeSinceLastUpdate < UpdateInterval)
		{
			return;
		}
		TimeSinceLastUpdate = 0.0f;
	}

	UpdateCurrentInteractable();
}

bool UNGCharacterInteractComponent::TryInteract_Implementation()
{
	if (CurrentInteractable && INGInteractionInterface::Execute_IsReadyToInteract(CurrentInteractable.GetObject()))
	{
		INGInteractionInterface::Execute_Interact(CurrentInteractable.GetObject(), GetOwner());
		return true;
	}
	return false;
}



UObject* UNGCharacterInteractComponent::GetCurrentInteractable_Implementation()
{
	return CurrentInteractable.GetObject();
}


UShapeComponent* UNGCharacterInteractComponent::GetInteractionShape() const
{
	if (AActor* Owner = GetOwner())
	{
		return Cast<UShapeComponent>(InteractionShapeRef.GetComponent(Owner));
	}
	return nullptr;
}

UCameraComponent* UNGCharacterInteractComponent::GetInteractionCamera() const
{
	if (AActor* Owner = GetOwner())
	{
		return Cast<UCameraComponent>(InteractionCameraRef.GetComponent(Owner));
	}
	return nullptr;
}


void UNGCharacterInteractComponent::OnInteractionShapeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetOwner() && OtherActor->Implements<UNGInteractionInterface>())
	{
		// Physical overlap is necessary but not sufficient — readiness drives
		// the visual chain. SyncReadyInteractables (called from the next tick)
		// will promote this actor and fire OnEnteredInteractRange only if its
		// IsReadyToInteract currently returns true.
		OverlappingInteractables.AddUnique(OtherActor);
	}
}

void UNGCharacterInteractComponent::OnInteractionShapeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	OverlappingInteractables.Remove(OtherActor);

	// Only fire Exit if the actor was actually in the ready set — actors that
	// were overlapping but never ready never showed an overlay, so there's
	// nothing to tear down. Mirror Sync's "demote then exit" order so the
	// material chain reverts cleanly.
	const bool bWasReady = ReadyInteractables.Remove(OtherActor) > 0;
	if (bWasReady && OtherActor->Implements<UNGInteractionInterface>())
	{
		if (CurrentInteractable.GetObject() == OtherActor)
		{
			INGInteractionInterface::Execute_OnDeselectedForInteract(OtherActor, GetOwner());
			CurrentInteractable = nullptr;
		}
		INGInteractionInterface::Execute_OnExitedInteractRange(OtherActor, GetOwner());
	}
	else if (CurrentInteractable.GetObject() == OtherActor)
	{
		CurrentInteractable = nullptr;
	}
}

void UNGCharacterInteractComponent::UpdateCurrentInteractable()
{
	// Reconcile readiness before picking — an actor whose IsReadyToInteract
	// has flipped since last tick (e.g. a panel just became occupied) will be
	// promoted/demoted here, firing the appropriate Enter/Exit events so the
	// overlay material chain tracks runtime state rather than raw collision.
	SyncReadyInteractables();

	AActor* BestActor = GetBestInteractableActor();
	UObject* PrevObject = CurrentInteractable.GetObject();

	if (BestActor == PrevObject)
	{
		return;
	}

	// Demote previous best back to "ready" if it is still ready. Sync already
	// handled the not-ready transition (deselect + exit), so here we only need
	// to deselect actors that are still in the ready set but no longer best.
	if (PrevObject)
	{
		AActor* PrevActor = Cast<AActor>(PrevObject);
		if (PrevActor && ReadyInteractables.Contains(PrevActor)
			&& PrevActor->Implements<UNGInteractionInterface>())
		{
			INGInteractionInterface::Execute_OnDeselectedForInteract(PrevActor, GetOwner());
		}
	}

	CurrentInteractable = BestActor;

	if (BestActor)
	{
		INGInteractionInterface::Execute_OnSelectedForInteract(BestActor, GetOwner());
	}
}

void UNGCharacterInteractComponent::SyncReadyInteractables()
{
	// First pass: drop actors that have left overlap or flipped to not-ready.
	// Walking backwards lets us RemoveAt in place.
	for (int32 i = ReadyInteractables.Num() - 1; i >= 0; --i)
	{
		AActor* Actor = ReadyInteractables[i];
		const bool bStillOverlapping = Actor && OverlappingInteractables.Contains(Actor);
		const bool bStillReady = bStillOverlapping
			&& Actor->Implements<UNGInteractionInterface>()
			&& INGInteractionInterface::Execute_IsReadyToInteract(Actor);

		if (!bStillReady)
		{
			if (Actor && Actor->Implements<UNGInteractionInterface>())
			{
				// Deselect first if this was the current pick, so the overlay
				// chain reverts selected → in-range → cleared (not selected
				// → cleared → in-range, which leaves a stuck overlay).
				if (CurrentInteractable.GetObject() == Actor)
				{
					INGInteractionInterface::Execute_OnDeselectedForInteract(Actor, GetOwner());
					CurrentInteractable = nullptr;
				}
				INGInteractionInterface::Execute_OnExitedInteractRange(Actor, GetOwner());
			}
			ReadyInteractables.RemoveAt(i);
		}
	}

	// Second pass: promote overlapping actors that have flipped to ready.
	for (AActor* Actor : OverlappingInteractables)
	{
		if (!Actor || ReadyInteractables.Contains(Actor)) continue;
		if (!Actor->Implements<UNGInteractionInterface>()) continue;

		if (INGInteractionInterface::Execute_IsReadyToInteract(Actor))
		{
			ReadyInteractables.Add(Actor);
			INGInteractionInterface::Execute_OnEnteredInteractRange(Actor, GetOwner());
		}
	}
}

AActor* UNGCharacterInteractComponent::GetBestInteractableActor()
{
	if (ReadyInteractables.Num() == 0)
	{
		return nullptr;
	}

	AActor* BestActor = nullptr;
	float BestDotProduct = -1.0f;

	FVector CameraLocation;
	FVector CameraForward;
	const UCameraComponent* InteractCamera = GetInteractionCamera();

	if (InteractCamera)
	{
		CameraLocation = InteractCamera->GetComponentLocation();
		CameraForward = InteractCamera->GetForwardVector();
	}
	else
	{
		// Fallback to owner actor rotation if no camera set
		CameraLocation = GetOwner()->GetActorLocation();
		CameraForward = GetOwner()->GetActorForwardVector();
	}

	for (AActor* Actor : ReadyInteractables)
	{
		if (!Actor) continue;

		// Ask the interactable where its hit-zone is in world space — defaults
		// to GetActorLocation(), but subclasses can override (e.g. control
		// panels whose interact zone is offset from the actor origin).
		const FVector TargetLocation = INGInteractionInterface::Execute_GetInteractWorldLocation(Actor);
		FVector DirectionToActor = (TargetLocation - CameraLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(CameraForward, DirectionToActor);

		// Check if within angle threshold
		float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

		if (Angle <= InteractAngleThreshold)
		{
			if (DotProduct > BestDotProduct)
			{
				BestDotProduct = DotProduct;
				BestActor = Actor;
			}
		}
	}

	return BestActor;
}
