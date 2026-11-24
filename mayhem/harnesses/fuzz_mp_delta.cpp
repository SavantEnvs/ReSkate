// Delta-coded multiplayer streams: one receiver fed a sequence of length-prefixed messages.
#include "Extension/Multiplayer/Net/delta_codec.h"
#include <fuzzer/FuzzedDataProvider.h>
#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    using namespace dingosdk::multiplayer;
    FuzzedDataProvider fdp(data, size);
    DeltaReceiver receiver; // fresh per input
    while (fdp.remaining_bytes() > 0) {
        auto msg = fdp.ConsumeBytes<std::uint8_t>(fdp.ConsumeIntegralInRange<size_t>(0, 4096));
        bool missing = false;
        (void)receiver.receive(msg, missing, 0);
    }
    return 0;
}
