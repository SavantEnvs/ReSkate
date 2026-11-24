// Shader program / texture lookup tables merged from mods (base table + mod edits + stock).
#include "Engine/Resource/shader_lookup.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    namespace sh = dingosdk::frostbite::shader;
    FuzzedDataProvider fdp(data, size);
    const bool texture = fdp.ConsumeBool();
    const bool withStock = fdp.ConsumeBool();
    const size_t nedits = fdp.ConsumeIntegralInRange<size_t>(1, 3);
    std::vector<std::vector<std::byte>> store;
    store.reserve(2 + 2 * nedits + 2);
    auto take = [&](bool rest = false) -> std::span<const std::byte> {
        store.push_back(fuzzhelp::piece(fdp, rest));
        return store.back();
    };
    sh::Table base{take(), take()};
    std::vector<sh::Table> edits;
    bool importable[3]{};
    for (size_t i = 0; i < nedits; ++i) {
        importable[i] = fdp.ConsumeBool();
        auto r = take();
        auto m = take(i + 1 == nedits && !withStock);
        edits.push_back({r, m});
    }
    sh::Table stock{};
    if (withStock) { auto r = take(); stock = {r, take(true)}; }
    try {
        if (texture) {
            auto merged = sh::merge_texture_lookup(base, edits);
            (void)merged;
        } else {
            (void)sh::preserves_stock_programs(base, stock);
            (void)sh::same_program_graph(base, edits[0]);
            (void)sh::only_stock_aliases(base, stock);
            auto merged = sh::merge_program_lookup(base, edits, stock, std::span<const bool>(importable, nedits));
            sh::Table grown{merged.resource, merged.resourceMeta};
            (void)sh::grown_from(grown, base);
        }
    } catch (const std::exception&) {}
    return 0;
}
