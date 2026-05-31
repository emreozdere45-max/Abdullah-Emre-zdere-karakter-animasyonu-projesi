# N8RO Human Motion Submission

## Public GitHub Repository

Public repository link:

```text
https://github.com/emreozdere45-max/Abdullah-Emre-zdere-karakter-animasyonu-projesi
```

## Screen Capture Video

Video link:

```text
media/demo_video.mp4
```

## Implemented Motion States

This project focuses on two kinematic locomotion states:

- `Walk`: slower procedural gait cycle.
- `Run`: faster procedural gait cycle and the main demonstrated state in the GLB viewer.

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
- `Documentation/REPORT_Abdullah_Emre_Ozdere.md`: project report written for this submission.
- `missions/human_animation_loop.lua`: N8RO mission script used to request the running state and navigation target.
- `dist/n8ro_human_motion_plugin.dll`: prebuilt plugin DLL for convenience.
- `media/demo_video.mp4`: screen capture video of the character in the N8RO GLB viewer.

## Email Text

```text
Dear Dr. Guclu,

My N8RO Human Motion project submission is below.

GitHub repository:
https://github.com/emreozdere45-max/Abdullah-Emre-zdere-karakter-animasyonu-projesi

Screen capture video:
Included in the repository as media/demo_video.mp4

Implemented motion states:
The model focuses on Walk and Run motion states. The character is moved forward through the N8RO navigation API, while a custom C++ animation plugin generates kinematic joint-angle overrides for walking/running-style motion. The final demonstration mainly shows the Run state because it produced the clearest visible result in the GLB viewer.

Controlled joints:
leftHip, leftKnee, leftAnkle, rightHip, rightKnee, rightAnkle, leftShoulder, leftElbow, rightShoulder, rightElbow.

The model is kinematic only. It outputs joint angles and does not compute forces, torques, rigid-body dynamics, or contact physics.

Best regards,
Abdullah Emre Ozdere
```
