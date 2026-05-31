#pragma once

#include <plugin/IPlugin.h>

#include <string>

namespace arkheon::astsim {
class ModelFactoryRegistry;
}

namespace n8ro_assignment {

class HumanMotionPlugin final : public arkheon::astlib::IPlugin {
public:
    HumanMotionPlugin() = default;
    ~HumanMotionPlugin() override = default;

    [[nodiscard]] int getInterfaceVersion() const override;
    [[nodiscard]] arkheon::astlib::PluginMetadata getMetadata() const override;

    void initialize(arkheon::astlib::PluginContext& context) override;
    void tick(double dt) override;
    void shutdown() override;

private:
    bool initialized_ = false;
    bool shutdown_ = false;
    bool registeredAny_ = false;
    std::string modelType_ = "animationModelNathanHuman";
    arkheon::astsim::ModelFactoryRegistry* modelFactoryRegistry_ = nullptr;
};

} // namespace n8ro_assignment

extern "C" {
ARKHEON_ASTLIB_API arkheon::astlib::IPlugin* create_plugin();
ARKHEON_ASTLIB_API void destroy_plugin(arkheon::astlib::IPlugin* plugin);
ARKHEON_ASTLIB_API const char* get_plugin_signature();
}
