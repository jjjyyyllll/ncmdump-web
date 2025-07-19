#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <string>
#include "ncmcrypt.h"

using namespace emscripten;

val decryptNCM(const val &inputData, const std::string &outputBaseNameFromJS) {

    std::vector<uint8_t> input_data_vector = vecFromJSArray<uint8_t>(inputData);

    NeteaseCrypt crypt(input_data_vector.data(), input_data_vector.size());
    crypt.DumpToMemory();
    crypt.FixMetadata();

    const std::vector<uint8_t>& finalDecryptedData = crypt.getDecryptedAudioData();

    return val(typed_memory_view(finalDecryptedData.size(), finalDecryptedData.data()));
}

EMSCRIPTEN_BINDINGS(ncmdump_module) {
    function("decryptNCM", &decryptNCM);
}