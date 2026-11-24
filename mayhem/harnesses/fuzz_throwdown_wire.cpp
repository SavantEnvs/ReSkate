// Throwdown messages relayed by the server between players, and the flag body.
#include "Extension/Throwdowns/throwdown_wire.h"
#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::multiplayer;
    std::span<const std::uint8_t> in(data, size);
    if (auto m = decode_throwdown(in)) {
        auto e = encode_throwdown(*m);
        (void)decode_throwdown(e);
    }
    if (auto f = decode_flag_body(in)) {
        auto e = encode_flag_body(*f);
        (void)decode_flag_body(e);
    }
    return 0;
}
