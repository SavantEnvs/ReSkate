// Multiplayer packets as clients and ReSkateServer receive them: protocol decode, wire decode, re-encode.
#include "Extension/Multiplayer/Net/protocol.h"
#include "Extension/Multiplayer/Net/wire_codec.h"
#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::multiplayer;
    std::span<const std::uint8_t> in(data, size);
    if (auto p = decode(in)) {
        auto e = encode(*p);
        (void)decode(e);
    }
    if (auto w = decode_wire(in)) {
        auto e = encode_wire(*w);
        (void)decode_wire(e);
    }
    (void)decode_wire_bytes(in);
    return 0;
}
