# N8RO Human Motion Controller - Project Report

**Name:** Abdullah Emre Ozdere  
**Platform:** N8RO Simulation - C++ Plugin  
**Language:** C++17 and Lua  
**Target:** Windows x64

---

## 1. Overview

This project implements a kinematic human motion controller for the N8RO simulation platform. The main controller is written as a C++ DLL plugin and registered as a simulation plugin inside N8RO. A small Lua mission script is used only to request the motion state and move the character forward through the N8RO navigation API.

The project focuses on two locomotion states:

- Walk
- Run

During development, the main goal was to generate motion through joint angles rather than forces or rigid-body dynamics. The model computes procedural joint-angle values and sends them to the N8RO animation system as joint overrides.

---

## 2. Objective

The objective was to create a closed-library, kinematics-only motion controller for the N8RO human character. The intended behavior was:

- move the character forward in the N8RO scene,
- generate visible leg motion for walking/running,
- control 10 major human joints,
- keep the implementation limited to joint angles,
- avoid force, torque, rigid-body dynamics, and contact-physics calculations.

---

## 3. Implemented Motion States

### Walk

The Walk state was designed as a lower-frequency gait cycle. The intended structure was to move the left and right legs in opposite phases, with the hip, knee, and ankle joints producing a repeated step pattern.

### Run

The Run state was the main implemented and tested state. It uses a faster periodic cycle than walking. The character translation is requested through N8RO navigation, while the C++ plugin generates joint-angle overrides for the running pose.

The final demo focuses mainly on the Run state because it was the clearest motion state to connect with forward navigation inside the GLB viewer.

---

## 4. Controlled Joints

The model controls the following 10 joints:

| # | Joint | Role |
|---|-------|------|
| 1 | `leftHip` | Left upper-leg motion |
| 2 | `leftKnee` | Left knee flexion |
| 3 | `leftAnkle` | Left foot/ankle correction |
| 4 | `rightHip` | Right upper-leg motion |
| 5 | `rightKnee` | Right knee flexion |
| 6 | `rightAnkle` | Right foot/ankle correction |
| 7 | `leftShoulder` | Left arm posture/counter-motion |
| 8 | `leftElbow` | Left elbow bend |
| 9 | `rightShoulder` | Right arm posture/counter-motion |
| 10 | `rightElbow` | Right elbow bend |

All joint values are output as radians.

---

## 5. System Architecture

The project is split into two parts:

### C++ Plugin

The C++ plugin is responsible for the model itself. It registers with the N8RO plugin system using the `ARKHEON_PLUGIN_V1` interface and connects to the Nathan human animation model.

Main source files:

- `src/N8ROHumanMotionPlugin.cpp`
- `include/N8ROHumanMotionPlugin.h`

The plugin generates joint angles in real time using sine-wave based procedural motion. The left and right legs are phase-shifted so that they do not move together.

### Lua Mission Script

The Lua script requests the active animation state and sends navigation targets to move the character forward.

Included script:

- `missions/human_animation_loop.lua`

Lua is not responsible for the joint model in the final structure. It is used mainly for movement/navigation setup.

---

## 6. Animation Method

The locomotion controller uses periodic functions to generate joint angles:

```text
joint_angle = sin(time * frequency) * amplitude
```

For opposite leg motion:

```text
left_phase  = sin(time * frequency)
right_phase = -left_phase
```

The knee angle is increased when the corresponding leg is in its swing phase. This was intended to create a visible stepping/running motion.

---

## 7. Build Instructions

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The generated DLL is:

```text
build/Release/n8ro_human_motion_plugin.dll
```

A prebuilt DLL is included in:

```text
dist/n8ro_human_motion_plugin.dll
```

---

## 8. N8RO Setup

Copy the DLL:

```text
dist/n8ro_human_motion_plugin.dll
```

to:

```text
C:\N8RO\userPlugins\sim\n8ro_human_motion_plugin.dll
```

Copy the mission script:

```text
missions/human_animation_loop.lua
```

to:

```text
C:\N8RO\data\resources\missions\human_animation_loop.lua
```

Then restart N8RO, load `GenericCivilianPresence`, run the simulation, and open the GLB viewer.

---

## 9. Challenges

The most difficult part of the project was not writing the mathematical gait cycle itself, but making the generated joint angles appear correctly on the character in the N8RO GLB viewer.

I had significant difficulty with:

- matching logical joint names to the visible skeleton,
- finding the correct X/Y/Z rotation axes,
- making knee motion appear visibly and consistently,
- separating navigation movement from visual joint animation,
- avoiding unwanted side-to-side leg or arm swinging.

I spent a lot of time testing different joint mappings, axis choices, and angle values. In particular, I struggled to make the knee movement look correct. The character navigation movement works, but the visual leg animation did not always reach the quality I wanted.

Although the final result has limitations, I made a serious effort to build the required model-driven structure: a C++ DLL plugin, N8RO plugin registration, a Lua navigation script, and documentation of the controlled joints and implemented motion states.

---

## 10. Result

The project provides a C++ N8RO plugin and Lua mission script for a kinematic Walk/Run-style human motion controller. The final focus is on the Run state. The character can be moved forward using N8RO navigation, and the model outputs joint-angle overrides for the 10 required joints.

The implementation remains kinematics-only and does not use force, torque, rigid-body dynamics, or contact-physics calculations.
