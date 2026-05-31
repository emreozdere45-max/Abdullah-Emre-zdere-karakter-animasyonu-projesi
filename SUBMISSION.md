# N8RO Human Motion Submission

## Public GitHub Repository

Public repository link:

```text
PASTE_GITHUB_REPOSITORY_LINK_HERE
```

## Screen Capture Video

Video link:

```text
PASTE_VIDEO_LINK_HERE
```

## Implemented Motion State

This project implements one active motion state:

- `Run`: the character is moved forward through the N8RO navigation API, while the C++ plugin outputs kinematic joint-angle overrides for a running-style motion.

The model is kinematic only. It outputs joint angles and does not compute forces, torques, rigid-body dynamics, or contact physics.

## Controlled 10 Joints

The model controls these 10 joints:

| # | Joint |
| --- | --- |
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

## Main Files

- `src/N8ROHumanMotionPlugin.cpp`: C++ N8RO simulation plugin.
- `include/N8ROHumanMotionPlugin.h`: plugin interface.
- `CMakeLists.txt`: build configuration.
- `README.md`: build and integration notes.
- `missions/human_animation_loop.lua`: N8RO mission script used to request the running state and navigation target.
- `dist/n8ro_human_motion_plugin.dll`: prebuilt plugin DLL for convenience.

## Email Text

```text
Dear Dr. Guclu,

My N8RO Human Motion project submission is below.

GitHub repository:
PASTE_GITHUB_REPOSITORY_LINK_HERE

Screen capture video:
PASTE_VIDEO_LINK_HERE

Implemented motion state:
The model implements a Run state. The character is moved forward through the N8RO navigation API, while a custom C++ animation plugin generates kinematic joint-angle overrides for a running-style motion.

Controlled joints:
leftHip, leftKnee, leftAnkle, rightHip, rightKnee, rightAnkle, leftShoulder, leftElbow, rightShoulder, rightElbow.

The model is kinematic only. It outputs joint angles and does not compute forces, torques, rigid-body dynamics, or contact physics.

Best regards,
YOUR_NAME
```
