// CAS block streams (raw / zlib / zstd / lz4 blocks; Oodle throws on Linux by design).
#include "Engine/Resource/cas_codec.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite;
    CasDecodeOptions o;
    o.maximumOutputSize = 8u << 20; // the library's own output cap, lowered from 256 MiB to keep RSS bounded
    try { (void)decode_cas(fuzzhelp::bytes(data, size), o); } catch (const std::exception&) {}
    return 0;
}
