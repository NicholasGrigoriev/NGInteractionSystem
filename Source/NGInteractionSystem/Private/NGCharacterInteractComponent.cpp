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
		const int32 PrevCount = OverlappingInteractables.Num();
		OverlappingInteractables.AddUnique(OtherActor);
		// AddUnique returns the index, not a "was-it-new" flag — derive newness
		// from count growth so we only fire the event on a real first entry.
		if (OverlappingInteractables.Num() > PrevCount)
		{
			INGInteractionInterface::Execute_OnEnteredInteractRange(OtherActor, GetOwner());
		}
	}
}

void UNGCharacterInteractComponent::OnInteractionShapeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	const int32 Removed = OverlappingInteractables.Remove(OtherActor);
	if (Removed > 0 && OtherActor->Implements<UNGInteractionInterface>())
	{
		INGInteractionInterface::Execute_OnExitedInteractRange(OtherActor, GetOwner());
	}

	if (CurrentInteractable.GetObject() == OtherActor)
	{
		// OnExitedInteractRange already cleared the visual state; just clear
		// the bookkeeping pointer here.
		CurrentInteractable = nullptr;
	}
}

void UNGCharacterInteractComponent::UpdateCurrentInteractable()
{
	AActor* BestActor = GetBestInteractableActor();
	UObject* PrevObject = CurrentInteractable.GetObject();

	if (BestActor == PrevObject)
	{
		return;
	}

	// Demote previous best back to "ready" if it is still in range. If the
	// previous best left range entirely, OnExitedInteractRange already fired
	// from the overlap-end handler and cleared its visuals — don't double-fire.
	if (PrevObject)
	{
		AActor* PrevActor = Cast<AActor>(PrevObject);
		if (PrevActor && OverlappingInteractables.Contains(PrevActor)
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

AActor* UNGCharacterInteractComponent::GetBestInteractableActor()
{
	if (OverlappingInteractables.Num() == 0)
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

	for (AActor* Actor : OverlappingInteractables)
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
