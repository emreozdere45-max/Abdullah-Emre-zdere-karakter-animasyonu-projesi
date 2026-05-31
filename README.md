# N8RO Human Motion Controller

This project implements a closed-library kinematic controller plugin for the N8RO human motion assignment.

The N8RO runtime plugin:

- uses a CoM-balance proxy from the gait phase exposed to the animation evaluator,
- consumes animation state supplied by the simulation engine,
- produces exactly 10 joint angles,
- uses joint angles only, with no force, torque, rigid-body dynamics, or contact-physics calculation.
- registers itself against the Nathan human animation model used by `GenericCivilianPresence`.
- overrides the `Idle Shake` animation code as the active `Run` state.

## Joint Output Order

All angles are in radians.

| Index | Joint |
| --- | --- |
| 0 | Left hip pitch |
| 1 | Left knee pitch |
| 2 | Left ankle pitch |
| 3 | Right hip pitch |
| 4 | Right knee pitch |
| 5 | Right ankle pitch |
| 6 | Left shoulder pitch |
| 7 | Left elbow pitch |
| 8 | Right shoulder pitch |
| 9 | Right elbow pitch |

## Build on Windows

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The N8RO plugin DLL is generated here:

```text
build/Release/n8ro_human_motion_plugin.dll
```

It has also been deployed to the N8RO simulation plugin folder:

```text
C:\N8RO\userPlugins\sim\n8ro_human_motion_plugin.dll
```

Restart N8RO after copying the plugin so the runtime can load it.

## Run the Demo

```powershell
.\build\Release\n8ro_motion_demo.exe
```

If your CMake generator is single-config, use:

```powershell
.\build\n8ro_motion_demo.exe
```

## Integration Surface

The N8RO integration entry points are declared in `include/N8ROHumanMotionPlugin.h`:

- `create_plugin`
- `destroy_plugin`
- `get_plugin_signature`

The plugin registers an evaluator on `animationModelNathanHuman` for:

- `Idle Shake` as `Run`

The evaluator returns 10 sparse joint overrides in radians:

- `leftHip`
- `leftKnee`
- `leftAnkle`
- `rightHip`
- `rightKnee`
- `rightAnkle`
- `leftShoulder`
- `leftElbow`
- `rightShoulder`
- `rightElbow`

The standalone `n8ro_human_motion.dll` and `n8ro_motion_demo.exe` are kept only as a local test harness. N8RO uses `n8ro_human_motion_plugin.dll`.
