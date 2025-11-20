#include "NGCharacterInteractComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "NGInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UNGCharacterInteractComponent::UNGCharacterInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(this);
	InteractionBox->SetBoxExtent(FVector(100.f, 50.f, 50.f));
	InteractionBox->SetCollisionProfileName(TEXT("Trigger"));
}


// Called when the game starts
void UNGCharacterInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionBox)
	{
		InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &UNGCharacterInteractComponent::OnInteractionBoxBeginOverlap);
		InteractionBox->OnComponentEndOverlap.AddDynamic(this, &UNGCharacterInteractComponent::OnInteractionBoxEndOverlap);
	}
}


// Called every frame
void UNGCharacterInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCurrentInteractable();
}

bool UNGCharacterInteractComponent::TryInteract_Implementation()
{
	if (CurrentInteractable)
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

void UNGCharacterInteractComponent::SetInteractCamera(UCameraComponent* NewCamera)
{
	InteractCamera = NewCamera;
}

void UNGCharacterInteractComponent::OnInteractionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetOwner() && OtherActor->Implements<UNGInteractionInterface>())
	{
		OverlappingInteractables.AddUnique(OtherActor);
	}
}

void UNGCharacterInteractComponent::OnInteractionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

		// Notify new interactable
		if (CurrentInteractable)
		{
			INGInteractionInterface::Execute_ReadyToInteract(CurrentInteractable.GetObject());
			// Also call SelectedToInteract as it is the currently selected one
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
