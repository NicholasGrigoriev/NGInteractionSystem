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
		OverlappingInteractables.AddUnique(OtherActor);
	}
}

void UNGCharacterInteractComponent::OnInteractionShapeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		OverlappingInteractables.Remove(OtherActor);
		
		// If the leaving actor was the current interactable, clear it
		if (CurrentInteractable.GetObject() == OtherActor)
		{
			INGInteractionInterface::Execute_NotReadyToInteract(CurrentInteractable.GetObject());
			CurrentInteractable = nullptr;
		}
	}
}

void UNGCharacterInteractComponent::UpdateCurrentInteractable()
{
	AActor* BestActor = GetBestInteractableActor();

	if (BestActor != CurrentInteractable.GetObject())
	{
		// Notify old interactable
		if (CurrentInteractable)
		{
			INGInteractionInterface::Execute_NotReadyToInteract(CurrentInteractable.GetObject());
		}

		CurrentInteractable = BestActor;

		// Notify new interactable. IsReadyToInteract is the "you're now the
		// candidate" event (applies the ready material in the base class);
		// SelectedToInteract is fired right after as the "you're the best
		// match" event (applies the selected material, overwriting ready).
		// Subclasses can override either independently if they want distinct
		// behavior for the two states.
		if (CurrentInteractable)
		{
			INGInteractionInterface::Execute_IsReadyToInteract(CurrentInteractable.GetObject());
			INGInteractionInterface::Execute_SelectedToInteract(CurrentInteractable.GetObject());
		}
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

		FVector DirectionToActor = (Actor->GetActorLocation() - CameraLocation).GetSafeNormal();
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
