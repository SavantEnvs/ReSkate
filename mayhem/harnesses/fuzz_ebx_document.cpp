// EBX (RIFF) documents from mod files: read, write back, and the file GUID reader.
#include "Engine/Resource/ebx_document.h"
#include "Engine/Resource/ebx_writer.h"
#include "fuzz_common.h"
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite::ebx;
    const auto in = fuzzhelp::bytes(data, size);
    try {
        auto doc = read_document(in);
        auto out = write_document(doc);
        (void)read_document(out);
    } catch (const std::exception&) {}
    try { (void)read_file_guid(in); } catch (const std::exception&) {}
    return 0;
}
