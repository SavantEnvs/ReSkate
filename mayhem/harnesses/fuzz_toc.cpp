// Frostbite TOC files (superbundle tables of contents): read + verify.
#include "Engine/Resource/toc.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite;
    try {
        auto t = read_toc(fuzzhelp::bytes(data, size));
        verify_toc(t);
    } catch (const std::exception&) {}
    return 0;
}
