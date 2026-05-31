#include "N8ROHumanMotionPlugin.h"

#include <model/AnimationModel.h>
#include <model/ModelFactoryRegistry.h>
#include <plugin/IModelPluginService.h>
#include <plugin/IPluginServices.h>
#include <plugin/PluginContext.h>

#include <algorithm>
#include <cmath>
#include <string_view>
#include <unordered_set>

namespace n8ro_assignment {
namespace {

struct JointPose {
    double xRad = 0.0;
    double yRad = 0.0;
    double zRad = 0.0;
};

double clamp(double value, double low, double high) {
    return std::max(low, std::min(value, high));
}

std::unordered_set<std::string> collectJointIds(const arkheon::astsim::AnimationModelInput& input) {
    std::unordered_set<std::string> availableJointIds;
    availableJointIds.reserve(input.entity.joints.size());
    for (const auto& joint : input.entity.joints) {
        availableJointIds.insert(joint.jointId);
    }
    return availableJointIds;
}

void pushJoint(
    arkheon::astsim::AnimationModelOutput& output,
    const std::unordered_set<std::string>& availableJointIds,
    const char* jointId,
    JointPose pose) {
    if (jointId && (availableJointIds.empty() || availableJointIds.find(jointId) != availableJointIds.end())) {
        output.jointOverrides.push_back({jointId, pose.xRad, pose.yRad, pose.zRad});
    }
}

bool evaluateRunKinematics(
    const arkheon::astsim::AnimationModelInput& input,
    arkheon::astsim::AnimationModelOutput& output) {
    const auto availableJointIds = collectJointIds(input);
    const double gaitFrequency = 6.8;
    const double phase = input.simulationTimeSeconds * gaitFrequency;
    const double leftPhase = std::sin(phase);
    const double rightPhase = -leftPhase;
    const double leftLift = std::max(0.0, leftPhase);
    const double rightLift = std::max(0.0, rightPhase);

    const double hipAmplitude = 0.18;
    const double kneeAmplitude = 1.35;
    const double ankleAmplitude = 0.10;
    const double armAmplitude = 0.18;
    const double forwardLean = -0.04;

    const double leftHipX = clamp(forwardLean + hipAmplitude * leftPhase, -0.28, 0.28);
    const double rightHipX = clamp(forwardLean + hipAmplitude * rightPhase, -0.28, 0.28);
    const double leftKneeX = clamp(0.10 + kneeAmplitude * leftLift, 0.08, 1.55);
    const double rightKneeX = clamp(0.10 + kneeAmplitude * rightLift, 0.08, 1.55);
    const double leftAnkleX = clamp(-ankleAmplitude * leftPhase, -0.16, 0.16);
    const double rightAnkleX = clamp(-ankleAmplitude * rightPhase, -0.16, 0.16);
    const double leftShoulder = clamp(0.22 - armAmplitude * leftPhase, -0.25, 0.55);
    const double rightShoulder = clamp(0.22 - armAmplitude * rightPhase, -0.25, 0.55);
    const double leftElbow = 0.40 + 0.10 * rightLift;
    const double rightElbow = 0.40 + 0.10 * leftLift;

    output.clearExistingJointOverrides = true;
    output.jointOverrides.clear();
    output.jointOverrides.reserve(10);

    // Legs: X axis only. Y/Z stay zero to avoid side-to-side swinging.
    pushJoint(output, availableJointIds, "leftHip", {leftHipX, 0.0, 0.0});
    pushJoint(output, availableJointIds, "leftKnee", {leftKneeX, 0.0, 0.0});
    pushJoint(output, availableJointIds, "leftAnkle", {leftAnkleX, 0.0, 0.0});
    pushJoint(output, availableJointIds, "rightHip", {rightHipX, 0.0, 0.0});
    pushJoint(output, availableJointIds, "rightKnee", {rightKneeX, 0.0, 0.0});
    pushJoint(output, availableJointIds, "rightAnkle", {rightAnkleX, 0.0, 0.0});
    pushJoint(output, availableJointIds, "leftShoulder", {leftShoulder, 0.0, -1.45});
    pushJoint(output, availableJointIds, "leftElbow", {leftElbow, 0.0, 0.0});
    pushJoint(output, availableJointIds, "rightShoulder", {rightShoulder, 0.0, 1.45});
    pushJoint(output, availableJointIds, "rightElbow", {rightElbow, 0.0, 0.0});

    return !output.jointOverrides.empty();
}

bool registerAnimation(
    arkheon::astsim::IAnimationModel* animationModel,
    std::string_view animationCode) {
    return animationModel && animationModel->registerAnimation(animationCode, evaluateRunKinematics);
}

void unregisterAnimation(
    arkheon::astsim::IAnimationModel* animationModel,
    std::string_view animationCode) {
    if (animationModel) {
        static_cast<void>(animationModel->registerAnimation(
            animationCode,
            arkheon::astsim::IAnimationModel::AnimationEvaluationFunction {}));
    }
}

} // namespace

int HumanMotionPlugin::getInterfaceVersion() const {
    return 1;
}

arkheon::astlib::PluginMetadata HumanMotionPlugin::getMetadata() const {
    arkheon::astlib::PluginMetadata metadata;
    metadata.setPluginId("n8ro-human-motion-assignment");
    metadata.setVersion("1.0.0");
    metadata.setAuthor("N8RO Assignment");
    return metadata;
}

void HumanMotionPlugin::initialize(arkheon::astlib::PluginContext& context) {
    initialized_ = true;
    shutdown_ = false;
    registeredAny_ = false;
    modelFactoryRegistry_ = nullptr;

    if (context.services) {
        auto* rawService = context.services->getService(arkheon::astsim::IModelPluginService::kPluginServiceId);
        auto* service = static_cast<arkheon::astsim::IModelPluginService*>(rawService);
        modelFactoryRegistry_ = service ? &service->modelFactoryRegistry() : nullptr;
    }
    if (!modelFactoryRegistry_) {
        return;
    }

    auto* prototypeBase = modelFactoryRegistry_->getRegisteredPrototype(modelType_);
    auto* animationModel = dynamic_cast<arkheon::astsim::IAnimationModel*>(prototypeBase);
    if (!animationModel) {
        return;
    }

    registeredAny_ = registerAnimation(animationModel, "Idle Shake");
}

void HumanMotionPlugin::tick(double dt) {
    static_cast<void>(dt);
    if (!initialized_ || shutdown_) {
        return;
    }
}

void HumanMotionPlugin::shutdown() {
    if (modelFactoryRegistry_ && registeredAny_) {
        auto* prototypeBase = modelFactoryRegistry_->getRegisteredPrototype(modelType_);
        auto* animationModel = dynamic_cast<arkheon::astsim::IAnimationModel*>(prototypeBase);
        unregisterAnimation(animationModel, "Idle Shake");
    }
    registeredAny_ = false;
    shutdown_ = true;
    modelFactoryRegistry_ = nullptr;
}

} // namespace n8ro_assignment

extern "C" {

ARKHEON_ASTLIB_API arkheon::astlib::IPlugin* create_plugin() {
    return new n8ro_assignment::HumanMotionPlugin();
}

ARKHEON_ASTLIB_API void destroy_plugin(arkheon::astlib::IPlugin* plugin) {
    delete plugin;
}

ARKHEON_ASTLIB_API const char* get_plugin_signature() {
    return "ARKHEON_PLUGIN_V1";
}

} // extern "C"
