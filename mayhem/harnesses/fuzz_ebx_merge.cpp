// Mod merge of EBX assets: 2-3 independently edited copies combined onto the base
// (merge_documents), the material grid combiner, and writing the merged result.
#include "Engine/Resource/ebx_document.h"
#include "Engine/Resource/ebx_merge.h"
#include "Engine/Resource/ebx_writer.h"
#include "Engine/Resource/material_grid.h"
#include "fuzz_common.h"
#include <exception>
#include <optional>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    namespace ebx = dingosdk::frostbite::ebx;
    namespace mg = dingosdk::frostbite::material_grid;
    FuzzedDataProvider fdp(data, size);
    const size_t nedits = fdp.ConsumeIntegralInRange<size_t>(1, 2);
    std::vector<std::vector<std::byte>> blobs;
    for (size_t i = 0; i <= nedits; ++i) blobs.push_back(fuzzhelp::piece(fdp, i == nedits));
    std::vector<ebx::Document> docs;
    for (auto& b : blobs) {
        try { docs.push_back(ebx::read_document(b)); } catch (const std::exception&) {}
    }
    if (docs.size() < 2) return 0;
    std::vector<const ebx::Document*> edits;
    for (size_t i = 1; i < docs.size(); ++i) edits.push_back(&docs[i]);
    try {
        ebx::MergeSummary summary;
        auto merged = ebx::merge_documents(docs[0], edits, &summary);
        (void)ebx::write_document(merged);
    } catch (const std::exception&) {}
    try {
        mg::Combiner c(docs[0]);
        for (auto* e : edits) { try { (void)c.add(*e); } catch (const std::exception&) {} }
        (void)ebx::write_document(c.result());
    } catch (const std::exception&) {}
    return 0;
}
