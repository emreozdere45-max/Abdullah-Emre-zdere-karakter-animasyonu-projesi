#pragma once

#include <stddef.h>

#if defined(_WIN32)
#if defined(N8RO_MOTION_BUILD_DLL)
#define N8RO_MOTION_API __declspec(dllexport)
#else
#define N8RO_MOTION_API __declspec(dllimport)
#endif
#else
#define N8RO_MOTION_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum N8ROJointIndex {
    N8RO_LEFT_HIP_PITCH = 0,
    N8RO_LEFT_KNEE_PITCH = 1,
    N8RO_LEFT_ANKLE_PITCH = 2,
    N8RO_RIGHT_HIP_PITCH = 3,
    N8RO_RIGHT_KNEE_PITCH = 4,
    N8RO_RIGHT_ANKLE_PITCH = 5,
    N8RO_LEFT_SHOULDER_PITCH = 6,
    N8RO_LEFT_ELBOW_PITCH = 7,
    N8RO_RIGHT_SHOULDER_PITCH = 8,
    N8RO_RIGHT_ELBOW_PITCH = 9,
    N8RO_JOINT_COUNT = 10
};

typedef struct N8ROVec3 {
    double x;
    double y;
    double z;
} N8ROVec3;

typedef struct N8ROSegment {
    N8ROVec3 position;
    double mass;
} N8ROSegment;

typedef struct N8ROControllerInput {
    double time_seconds;
    double delta_seconds;
    double target_speed_mps;
    int left_foot_contact;
    int right_foot_contact;
    const N8ROSegment* segments;
    size_t segment_count;
} N8ROControllerInput;

typedef struct N8ROControllerOutput {
    double center_of_mass[3];
    double joint_angles_rad[N8RO_JOINT_COUNT];
} N8ROControllerOutput;

typedef void* N8ROControllerHandle;

N8RO_MOTION_API N8ROControllerHandle n8ro_controller_create(void);
N8RO_MOTION_API void n8ro_controller_destroy(N8ROControllerHandle handle);
N8RO_MOTION_API int n8ro_controller_step(
    N8ROControllerHandle handle,
    const N8ROControllerInput* input,
    N8ROControllerOutput* output);

#ifdef __cplusplus
}
#endif
