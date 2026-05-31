# N8RO Human Motion Project Report

**Student:** Abdullah Emre Ozdere  
**Project:** Kinematic Human Motion Controller  
**Platform:** N8RO Simulation Environment  
**Implementation:** C++ DLL plugin and Lua mission script

## Project Summary

This project implements a procedural human motion controller for the N8RO simulation platform. The goal was to move a human character using joint angles only, without calculating forces, torques, rigid-body dynamics, or contact physics.

The project focuses on two locomotion states:

- Walk
- Run

The final demonstration mainly shows the character moving forward with a running-style motion in the N8RO GLB viewer. The movement is supported by a custom C++ plugin and a Lua mission script.

## Implemented Approach

The C++ plugin generates procedural joint-angle values using periodic sine-wave based motion. The left and right legs are phase-shifted so that one leg moves forward while the other moves backward. This structure was used to approximate walking and running behavior.

The Lua script is used to request the active motion behavior and move the character forward through N8RO navigation. The Lua script does not calculate the joint model itself; the joint-angle logic is handled inside the C++ plugin.

## Controlled 10 Joints

The model controls the following 10 joints:

| # | Joint |
|---|-------|
| 1 | `leftHip` |
| 2 | `leftKnee` |
| 3 | `leftAnkle` |
| 4 | `rightHip` |
| 5 | `rightKnee` |
| 6 | `rightAnkle` |
| 7 | `leftShoulder` |
| 8 | `leftElbow` |
| 9 | `rightShoulder` |
| 10 | `rightElbow` |

These joints were selected because they are the main joints needed for a basic bipedal walking/running controller.

## Motion States

### Walk

The walking state was designed as a slower gait cycle. The hips, knees, and ankles are intended to move with a lower frequency and smaller amplitude.

### Run

The running state was designed as a faster gait cycle. The legs use a higher frequency and larger movement values to represent faster locomotion. The final video demonstration focuses mostly on this state because it produced the clearest visible result in the GLB viewer.

## Files Included

- `src/N8ROHumanMotionPlugin.cpp`: main C++ plugin implementation
- `include/N8ROHumanMotionPlugin.h`: plugin header/interface
- `missions/human_animation_loop.lua`: N8RO mission/navigation script
- `dist/n8ro_human_motion_plugin.dll`: prebuilt plugin DLL
- `media/demo_video.mp4`: screen capture video from the N8RO GLB viewer
- `README.md`: project explanation and setup notes
- `SUBMISSION.md`: short submission summary and email text

## Difficulties Encountered

The most difficult part of the project was making the generated joint angles appear correctly on the visible human model in the N8RO GLB viewer. I spent a lot of time testing different axis directions, joint names, and angle values.

The main problems I encountered were:

- matching the plugin joint names to the actual GLB skeleton,
- finding which axis produced forward/back leg movement,
- making the knee movement visible,
- preventing unwanted side-to-side swinging,
- connecting the visual animation with the character navigation movement.

Although the final result is not perfect, I made a serious effort to implement the required model-driven structure and to satisfy the project scope with a kinematic C++ plugin.

## Result

The final project contains a working N8RO-oriented C++ plugin structure, a Lua script for movement in the simulation environment, a prebuilt DLL, documentation, and a screen capture video. The implementation is focused on Walk/Run-style human motion and controls the required 10 joints using joint-angle outputs.
