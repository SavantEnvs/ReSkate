// Frostbite binary bundles (bundle manifests in mod packages): read, write back, region reader.
#include "Engine/Resource/binary_bundle.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite;
    const auto in = fuzzhelp::bytes(data, size);
    try {
        auto b = read_binary_bundle(in);
        auto out = write_binary_bundle(b);
        (void)read_binary_bundle(out);
    } catch (const std::exception&) {}
    try { (void)read_bundle_region(in, 0); } catch (const std::exception&) {}
    return 0;
}
