// Texture resources from mods: header + metadata, then decoding the chunk (bcdec block decoders).
#include "Engine/Resource/texture.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite;
    FuzzedDataProvider fdp(data, size);
    auto header = fuzzhelp::piece(fdp);
    auto meta = fuzzhelp::piece(fdp);
    auto chunk = fuzzhelp::piece(fdp, true);
    try {
        auto h = read_texture_header(header, meta);
        auto img = decode_texture(h, chunk, 64);
        (void)resize(img, 16, 16);
    } catch (const std::exception&) {}
    return 0;
}
