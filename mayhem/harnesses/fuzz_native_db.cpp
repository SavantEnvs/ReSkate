// Frostbite native DB trees (InitFS, layout.toc, bundle manifests): read and write back.
#include "Engine/Vfs/native_db.h"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    namespace db = dingosdk::native_db;
    std::span<const unsigned char> in(data, size);
    for (bool unique : {true, false}) {
        try {
            std::size_t used = 0;
            db::Options o;
            o.unique_fields = unique;
            auto n = db::read(in, "fuzz", &used, o);
            auto out = db::write(n);
            (void)db::read(out, "fuzz-rt", nullptr, o);
        } catch (const std::exception&) {}
    }
    return 0;
}
