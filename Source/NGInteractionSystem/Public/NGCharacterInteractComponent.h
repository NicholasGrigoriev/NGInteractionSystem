#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NGCharacterInteractInterface.h"
#include "NGCharacterInteractComponent.generated.h"

class UBoxComponent;
class UCameraComponent;
class INGInteractionInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NGINTERACTIONSYSTEM_API UNGCharacterInteractComponent : public USceneComponent, public INGCharacterInteractInterface
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

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractCamera(UCameraComponent* NewCamera);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UBoxComponent> InteractionBox;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UCameraComponent> InteractCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TScriptInterface<INGInteractionInterface> CurrentInteractable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractAngleThreshold = 15.0f;

	UFUNCTION()
	void OnInteractionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateCurrentInteractable();
	AActor* GetBestInteractableActor();

private:
	TArray<AActor*> OverlappingInteractables;
};
