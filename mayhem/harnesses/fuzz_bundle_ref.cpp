// *_bundlereftable resources merged from mods (costume presets -> bundles).
#include "Engine/Resource/bundle_ref_table.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    namespace br = dingosdk::frostbite::bundle_ref;
    FuzzedDataProvider fdp(data, size);
    const size_t nedits = fdp.ConsumeIntegralInRange<size_t>(1, 2);
    std::vector<std::vector<std::byte>> store;
    store.reserve(2 + 2 * nedits);
    auto take = [&](bool rest = false) -> std::span<const std::byte> {
        store.push_back(fuzzhelp::piece(fdp, rest));
        return store.back();
    };
    br::Table base{take(), take()};
    std::vector<br::Table> edits;
    for (size_t i = 0; i < nedits; ++i) { auto r = take(); edits.push_back({r, take(i + 1 == nedits)}); }
    try { (void)br::merge(base, edits); } catch (const std::exception&) {}
    return 0;
}
