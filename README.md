# NGInteractionSystem

A lightweight, reusable interaction plugin for Unreal Engine 5. Drop it into any project, wire up your character, and get proximity-based interaction with visual feedback — no dependencies on GAS or any specific game framework.

> **Article**: [NGInteractionSystem — Proximity-Based Interaction for UE5](https://digitalgarden-pink.vercel.app/NGInteractionSystem---Proximity-Based-Interaction-for-UE5)

## Features

- **Interface-based architecture** — any actor can become interactable by implementing `INGInteractionInterface`, no base class required
- **Two-phase detection** — a box collision trigger handles spatial filtering (event-driven), then an angle cone selects the best target using dot product math
- **Visual feedback** — overlay materials highlight interactable objects without modifying their base appearance
- **Configurable update rate** — `UpdateInterval` property lets you throttle detection updates (e.g. 0.1s for 10Hz polling instead of every frame)
- **Blueprint and C++ support** — all interface methods are `BlueprintNativeEvent`

## How Detection Works

1. A `UBoxComponent` trigger (default 100×50×50 units) detects actors entering/leaving the interaction zone via overlap events
2. Periodically, the component iterates tracked actors and calculates the angle between the camera's forward vector and each candidate
3. The actor closest to the screen center — within `InteractAngleThreshold` (default 15°) — becomes the active target
4. When the target changes, the old one gets `NotReadyToInteract()`, the new one gets `ReadyToInteract()` + `SelectedToInteract()`

## Installation

1. Clone into your project's `Plugins` folder:
    ```bash
    cd YourProject/Plugins
    git clone https://github.com/NicholasGrigoriev/NGInteractionSystem.git
    ```
2. Regenerate project files (right-click `.uproject` → Generate Visual Studio project files)
3. Compile and enable the plugin in Edit → Plugins

## Usage

### Setup Character

1. Add `UNGCharacterInteractComponent` to your Character (Blueprint or C++)
2. Call `SetInteractCamera` to specify which camera drives look-at calculations (defaults to actor forward vector if not set)
3. Bind an input action to call `TryInteract` on the component
4. Optionally set `UpdateInterval` to throttle detection updates (0 = every frame)

### Create Interactables

**Option A: Inherit from Base Class**
1. Create a Blueprint inheriting from `ANGInteractableActor`
2. Assign your mesh to the `InteractMesh` component
3. Set `InteractReadyMaterial` and `InteractSelectedMaterial` for visual feedback
4. Override `Interact` to implement custom logic

**Option B: Implement Interface**
1. Add `INGInteractionInterface` to any Actor
2. Implement: `Interact`, `IsReadyToInteract`, `ReadyToInteract`, `SelectedToInteract`, `NotReadyToInteract`

## Visual Feedback with Overlay Materials

The base class `ANGInteractableActor` uses Unreal's overlay material system to give players clear visual cues about what they can interact with. Overlay materials render on top of the object's base material without replacing it — the original appearance stays intact.

The system uses two material slots:

- **`InteractReadyMaterial`** — applied when the player is nearby and looking at the object. Typically a subtle highlight (e.g. a faint outline or glow) signaling "this is interactable."
- **`InteractSelectedMaterial`** — applied when the object becomes the active selection. Typically a stronger, colored overlay (e.g. green) telling the player "press the button to interact with this."

When the player looks away or moves out of range, the overlay is cleared and the object returns to its normal appearance.

To set this up, create two simple overlay materials (translucent or additive blending works best) and assign them in the Details panel of your `ANGInteractableActor` Blueprint. No material swapping, no dynamic material instances — just a clean overlay on top.

## Core Classes

| Class | Role |
|-------|------|
| `INGInteractionInterface` | Interface for interactable objects (5 methods) |
| `INGCharacterInteractInterface` | Interface for the interactor (TryInteract, GetCurrentInteractable) |
| `UNGCharacterInteractComponent` | Scene component driving detection and target selection |
| `ANGInteractableActor` | Convenience base class with overlay material support |

## License

MIT — see [LICENSE](LICENSE)
