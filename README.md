# NG Interaction System

A generic, reusable interaction system plugin for Unreal Engine.

## Features

- **Generic Interaction Interface**: `UNGInteractionInterface` allows any actor to become interactable.
- **Character Component**: `UNGCharacterInteractComponent` handles interaction logic, including detection and input.
- **Base Interactable Actor**: `ANGInteractableActor` provides a ready-to-use base class with built-in support for:
    - **Ready Material**: Visual feedback when the player looks at the object.
    - **Selected Material**: Visual feedback when the object is selected.
- **Flexible Detection**: Uses a Box Component trigger and camera look-at angle to determine the best interactable object.

## Installation

1.  Clone this repository into your project's `Plugins` folder:
    ```bash
    cd YourProject/Plugins
    git clone https://github.com/NicholasGrigoriev/NGInteractionSystem.git NG_InteractionSystem
    ```
2.  Regenerate your project files (Right-click `.uproject` -> Generate Visual Studio project files).
3.  Compile your project.
4.  Enable the plugin in the Unreal Editor (Edit -> Plugins -> NG_InteractionSystem).

## Usage

### Setup Character

1.  Add the `UNGCharacterInteractComponent` to your Character Blueprint or C++ class.
2.  (Optional) Call `SetInteractCamera` to specify which camera to use for look-at calculations. If not set, it defaults to the actor's forward vector.
3.  Bind an input action (e.g., "Interact") to call `TryInteract` on the component.

### Create Interactables

**Option A: Inherit from Base Class**
1.  Create a new Blueprint inheriting from `ANGInteractableActor`.
2.  Assign your mesh to the `InteractMesh` component.
3.  Set the `InteractReadyMaterial` and `InteractSelectedMaterial` in the Details panel to define visual feedback.
4.  Override `Interact` event to implement your custom logic.

**Option B: Implement Interface**
1.  Add `UNGInteractionInterface` to any Actor.
2.  Implement the `Interact`, `ReadyToInteract`, `SelectedToInteract`, and `NotReadyToInteract` functions.

## Architecture

- **Module**: `NGInteractionSystem`
- **Prefix**: `NG` (to avoid naming collisions)

### Core Classes
- `UNGInteractionInterface`: The interface for interactable objects.
- `INGCharacterInteractInterface`: The interface for the interactor.
- `UNGCharacterInteractComponent`: The component that drives interaction.
- `ANGInteractableActor`: A convenience base class.

## License

MIT
