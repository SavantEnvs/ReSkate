// EBX type-fixup repair run on every mod's EBX during the merge, then the repaired document.
#include "Engine/Resource/ebx_fixup_repair.h"
#include "Engine/Resource/ebx_document.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite::ebx;
    try {
        auto r = repair_type_fixups(fuzzhelp::bytes(data, size));
        if (r) (void)read_document(r->bytes);
    } catch (const std::exception&) {}
    return 0;
}
