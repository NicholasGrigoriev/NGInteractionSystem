#include "NGInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"

// Sets default values
ANGInteractableActor::ANGInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractMesh"));
	RootComponent = InteractMesh;

	bIsInteractable = true;
}

// Called when the game starts or when spawned
void ANGInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANGInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANGInteractableActor::Interact_Implementation(AActor* InteractingActor)
{
	if (!bIsInteractable) return;
	
	// Base implementation can be empty or log something
	UE_LOG(LogTemp, Log, TEXT("Interacted with %s"), *GetName());
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
