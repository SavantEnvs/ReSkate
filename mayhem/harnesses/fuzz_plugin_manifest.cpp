// plugin.json manifests shipped inside downloaded plugin packages, and package-relative paths.
#include "Extension/Plugins/plugin_manifest.h"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::plugins;
    std::string_view text(reinterpret_cast<const char*>(data), size);
    try { (void)parse_manifest(text); } catch (const std::exception&) {}
    try { (void)safe_relative_path(text); } catch (const std::exception&) {}
    return 0;
}
