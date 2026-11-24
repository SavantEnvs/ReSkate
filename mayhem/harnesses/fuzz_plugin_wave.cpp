// WAV sounds shipped in plugin packages: RIFF/WAVE chunk walk and PCM conversion.
#include "Extension/Plugins/plugin_wave.h"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::plugins;
    std::string error;
    try { (void)decode_wave({data, size}, 5.0f, error); } catch (const std::exception&) {}
    return 0;
}
