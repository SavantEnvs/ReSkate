// MeshSet resources from mods: LOD metadata, geometry layout checks, skinned mesh reader.
#include "Engine/Resource/mesh_set.h"
#include "fuzz_common.h"
#include <algorithm>
#include <exception>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::frostbite;
    FuzzedDataProvider fdp(data, size);
    auto resource = fuzzhelp::piece(fdp);
    auto geometry = fuzzhelp::piece(fdp, true);
    std::size_t n = 0;
    try { n = mesh_lod_count(resource); } catch (const std::exception&) { n = 1; }
    n = std::min<std::size_t>(n, 4);
    for (std::size_t lod = 0; lod < n; ++lod) {
        try { (void)read_mesh_lod_metadata(resource, lod); } catch (const std::exception&) {}
        try { (void)read_mesh_lod(resource, lod); } catch (const std::exception&) {}
        try { (void)mesh_lod_geometry(resource, lod); } catch (const std::exception&) {}
        try { std::size_t l = 0; (void)any_mesh_lod_geometry(resource, lod, &l); } catch (const std::exception&) {}
        try { std::uint32_t f = 0; (void)check_mesh_lod(resource, lod, geometry, &f); } catch (const std::exception&) {}
        try { (void)read_mesh_positions(resource, lod, geometry); } catch (const std::exception&) {}
        try { (void)read_skinned_mesh(resource, lod, geometry); } catch (const std::exception&) {}
    }
    return 0;
}
