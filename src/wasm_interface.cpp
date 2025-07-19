#include <vector>
#include <string>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "ncmcrypt.h"

using namespace emscripten;

val decryptNCM(uintptr_t inputDataPtr, size_t inputDataSize) {
    const uint8_t* input_data_raw = reinterpret_cast<const uint8_t*>(inputDataPtr);

    NeteaseCrypt crypt(input_data_raw, inputDataSize);
    crypt.DumpToMemory();
    crypt.FixMetadata();

    const std::vector<uint8_t>& finalDecryptedData = crypt.getDecryptedAudioData();

    return val(typed_memory_view(finalDecryptedData.size(), finalDecryptedData.data()));
}

EMSCRIPTEN_BINDINGS(ncmdump_module) {
    function("decryptNCM", &decryptNCM);
}