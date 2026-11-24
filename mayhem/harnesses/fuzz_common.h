// Shared helpers for the ReSkate fuzz harnesses.
#pragma once
#include <fuzzer/FuzzedDataProvider.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace fuzzhelp {
inline std::span<const std::byte> bytes(const uint8_t* data, size_t size) {
    return {reinterpret_cast<const std::byte*>(data), size};
}
// A length-prefixed piece of the input (the last piece takes the rest when asked).
inline std::vector<std::byte> piece(FuzzedDataProvider& fdp, bool rest = false) {
    std::vector<uint8_t> v = rest ? fdp.ConsumeRemainingBytes<uint8_t>()
                                  : fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, 1 << 16));
    std::vector<std::byte> out(v.size());
    for (size_t i = 0; i < v.size(); ++i) out[i] = static_cast<std::byte>(v[i]);
    return out;
}
} // namespace fuzzhelp
