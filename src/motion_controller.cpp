#include "n8ro_motion_controller.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kTwoPi = 2.0 * kPi;
constexpr double kMinDt = 1.0 / 240.0;
constexpr double kMaxDt = 1.0 / 15.0;

double clamp(double value, double low, double high) {
    return std::max(low, std::min(value, high));
}

double sanitize(double value, double fallback) {
    return std::isfinite(value) ? value : fallback;
}

double smoothstep(double edge0, double edge1, double value) {
    const double t = clamp((value - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

N8ROVec3 compute_center_of_mass(const N8ROControllerInput& input) {
    N8ROVec3 weighted_sum{0.0, 0.0, 0.0};
    double total_mass = 0.0;

    for (size_t i = 0; i < input.segment_count; ++i) {
        const N8ROSegment& segment = input.segments[i];
        const double mass = sanitize(segment.mass, 0.0);
        if (mass <= 0.0) {
            continue;
        }

        weighted_sum.x += sanitize(segment.position.x, 0.0) * mass;
        weighted_sum.y += sanitize(segment.position.y, 0.0) * mass;
        weighted_sum.z += sanitize(segment.position.z, 0.0) * mass;
        total_mass += mass;
    }

    if (total_mass <= std::numeric_limits<double>::epsilon()) {
        return N8ROVec3{0.0, 1.0, 0.0};
    }

    return N8ROVec3{
        weighted_sum.x / total_mass,
        weighted_sum.y / total_mass,
        weighted_sum.z / total_mass,
    };
}

class MotionController {
public:
    int step(const N8ROControllerInput& input, N8ROControllerOutput& output) {
        const double dt = clamp(sanitize(input.delta_seconds, kMinDt), kMinDt, kMaxDt);
        const double desired_speed = clamp(sanitize(input.target_speed_mps, 1.1), 0.0, 2.2);
        const N8ROVec3 com = compute_center_of_mass(input);

        phase_ = std::fmod(phase_ + dt * gait_frequency(desired_speed), 1.0);
        if (phase_ < 0.0) {
            phase_ += 1.0;
        }

        const double contact_bias = contact_phase_bias(input.left_foot_contact, input.right_foot_contact);
        const double phase = std::fmod(phase_ + contact_bias + 1.0, 1.0);
        const double omega = kTwoPi * phase;
        const double speed_scale = smoothstep(0.05, 1.4, desired_speed);
        const double lateral_balance = clamp(com.x * 0.22, -0.10, 0.10);
        const double forward_balance = clamp((com.z - 0.03) * 0.35, -0.12, 0.12);

        const double left_leg = std::sin(omega);
        const double right_leg = -left_leg;
        const double left_swing = std::max(0.0, left_leg);
        const double right_swing = std::max(0.0, right_leg);

        std::array<double, N8RO_JOINT_COUNT> joints{};
        joints[N8RO_LEFT_HIP_PITCH] = clamp(0.40 * speed_scale * left_leg - forward_balance - lateral_balance, -0.55, 0.55);
        joints[N8RO_RIGHT_HIP_PITCH] = clamp(0.40 * speed_scale * right_leg - forward_balance + lateral_balance, -0.55, 0.55);

        joints[N8RO_LEFT_KNEE_PITCH] = clamp(0.12 + 0.62 * speed_scale * left_swing, 0.02, 0.90);
        joints[N8RO_RIGHT_KNEE_PITCH] = clamp(0.12 + 0.62 * speed_scale * right_swing, 0.02, 0.90);

        joints[N8RO_LEFT_ANKLE_PITCH] = clamp(-0.22 * speed_scale * left_leg + 0.5 * forward_balance, -0.35, 0.35);
        joints[N8RO_RIGHT_ANKLE_PITCH] = clamp(-0.22 * speed_scale * right_leg + 0.5 * forward_balance, -0.35, 0.35);

        joints[N8RO_LEFT_SHOULDER_PITCH] = clamp(-0.34 * speed_scale * left_leg, -0.50, 0.50);
        joints[N8RO_RIGHT_SHOULDER_PITCH] = clamp(-0.34 * speed_scale * right_leg, -0.50, 0.50);
        joints[N8RO_LEFT_ELBOW_PITCH] = clamp(0.35 + 0.12 * speed_scale * right_swing, 0.18, 0.65);
        joints[N8RO_RIGHT_ELBOW_PITCH] = clamp(0.35 + 0.12 * speed_scale * left_swing, 0.18, 0.65);

        output.center_of_mass[0] = com.x;
        output.center_of_mass[1] = com.y;
        output.center_of_mass[2] = com.z;

        for (size_t i = 0; i < joints.size(); ++i) {
            output.joint_angles_rad[i] = joints[i];
        }

        return 1;
    }

private:
    static double gait_frequency(double speed) {
        return 0.65 + 0.35 * clamp(speed, 0.0, 2.2);
    }

    static double contact_phase_bias(int left_contact, int right_contact) {
        if (left_contact && !right_contact) {
            return 0.02;
        }
        if (!left_contact && right_contact) {
            return 0.52;
        }
        return 0.0;
    }

    double phase_ = 0.0;
};

} // namespace

extern "C" {

N8RO_MOTION_API N8ROControllerHandle n8ro_controller_create(void) {
    try {
        return new MotionController();
    } catch (...) {
        return nullptr;
    }
}

N8RO_MOTION_API void n8ro_controller_destroy(N8ROControllerHandle handle) {
    delete static_cast<MotionController*>(handle);
}

N8RO_MOTION_API int n8ro_controller_step(
    N8ROControllerHandle handle,
    const N8ROControllerInput* input,
    N8ROControllerOutput* output) {
    if (handle == nullptr || input == nullptr || output == nullptr) {
        return 0;
    }
    if (input->segments == nullptr && input->segment_count != 0U) {
        return 0;
    }

    try {
        return static_cast<MotionController*>(handle)->step(*input, *output);
    } catch (...) {
        return 0;
    }
}

} // extern "C"
