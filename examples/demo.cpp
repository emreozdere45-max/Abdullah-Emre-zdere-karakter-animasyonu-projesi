#include "n8ro_motion_controller.h"

#include <iomanip>
#include <iostream>

int main() {
    N8ROControllerHandle controller = n8ro_controller_create();
    if (controller == nullptr) {
        std::cerr << "Controller creation failed\n";
        return 1;
    }

    const N8ROSegment segments[] = {
        {{0.00, 1.05, 0.00}, 35.0},
        {{-0.10, 0.55, 0.02}, 8.0},
        {{0.10, 0.55, -0.02}, 8.0},
        {{-0.22, 1.20, 0.01}, 4.0},
        {{0.22, 1.20, -0.01}, 4.0},
        {{0.00, 1.55, 0.00}, 6.0},
    };

    N8ROControllerInput input{};
    input.delta_seconds = 1.0 / 60.0;
    input.target_speed_mps = 1.1;
    input.segments = segments;
    input.segment_count = sizeof(segments) / sizeof(segments[0]);

    std::cout << std::fixed << std::setprecision(3);
    for (int frame = 0; frame < 12; ++frame) {
        input.time_seconds = frame * input.delta_seconds;
        input.left_foot_contact = (frame / 6) % 2 == 0;
        input.right_foot_contact = !input.left_foot_contact;

        N8ROControllerOutput output{};
        if (!n8ro_controller_step(controller, &input, &output)) {
            std::cerr << "Controller step failed\n";
            n8ro_controller_destroy(controller);
            return 1;
        }

        std::cout << "frame " << frame
                  << " com=(" << output.center_of_mass[0]
                  << ", " << output.center_of_mass[1]
                  << ", " << output.center_of_mass[2] << ")"
                  << " leftHip=" << output.joint_angles_rad[N8RO_LEFT_HIP_PITCH]
                  << " rightHip=" << output.joint_angles_rad[N8RO_RIGHT_HIP_PITCH]
                  << '\n';
    }

    n8ro_controller_destroy(controller);
    return 0;
}
