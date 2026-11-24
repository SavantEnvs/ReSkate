// JSON answers fetched over HTTPS by the server/launcher: GitHub release answers + launcher.json
// (server self-update), tar listings of the release tarball, and Thunderstore package answers.
// The first byte selects the parser; the rest is the answer text.
#include "Server/server_release.h"
#include "Engine/Vfs/thunderstore_package.h"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 1) return -1;
    const std::string_view text(reinterpret_cast<const char*>(data + 1), size - 1);
    namespace sv = dingosdk::server;
    namespace ts = dingosdk::thunderstore_package;
    try {
        switch (data[0] % 4) {
        case 0: {
            auto assets = sv::release_assets(text);
            (void)sv::parse_linux_release(text, assets);
            break;
        }
        case 1: {
            std::map<std::string, std::string, std::less<>> assets{
                {"ReSkateServer-linux.tar.gz", "https://github.com/x/y/releases/download/v1/ReSkateServer-linux.tar.gz"}};
            (void)sv::parse_linux_release(text, assets);
            (void)sv::https_url(text);
            break;
        }
        case 2: (void)sv::checked_archive_listing(text); break;
        case 3: {
            auto name = ts::parse_name("Sandos-Vancouver_Plaza");
            if (name) {
                (void)ts::choose(text, *name, "reskate", ts::Kind::map);
                (void)ts::choose(text, *name, "reskate", ts::Kind::plugin);
            }
            break;
        }
        }
    } catch (const std::exception&) {}
    return 0;
}
