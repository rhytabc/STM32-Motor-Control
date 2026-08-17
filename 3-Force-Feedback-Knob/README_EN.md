<div align="center">

[简体中文](./README.md) · **English**

</div>

# Force Feedback Knob (Spring Feel)

Based on the 2804 BLDC motor + SimpleFOC, this turns a motor into an "auto-centering" knob — turn it and it springs back; release it and it rests at center.

This is the integration of everything before: **torque control + encoder feedback + FOC**.

## Hardware

Identical to the [BLDC motor project](../2-BLDC-SimpleFOC/), no extra hardware required.

## Principle

```
Encoder reads current angle
    ↓
Force = -K × (current angle - center angle)
    ↓
Motor outputs this force (pushing back)
```

The core is one line:

```cpp
float torque = -spring_K * (angle - center_angle);
motor.move(torque);
```

- `spring_K` is the spring stiffness: higher = stiffer, lower = softer
- `center_angle` is the return-to-center point

## Serial Commands

| Command | Function |
|---|---|
| `K20` | Set spring stiffness (higher = stiffer) |
| `Z` | Set current position as center |

## Experience

1. After flashing, the motor's power-on position is automatically set as center
2. Turn the motor housing by hand → release → it springs back to center
3. Send `K2` for a soft spring, `K20` for a stiff spring

## Extension Directions

Based on the spring force formula, more feels can be simulated:

| Feel | Formula |
|---|---|
| Spring | `-K × (angle - center)` |
| Damping | `-D × velocity` |
| Detent | Add "step force" at fixed angles |
| End-stop | Push back hard beyond angle limit |

These are the core technologies behind car knobs, game controllers, and high-end audio knobs.
