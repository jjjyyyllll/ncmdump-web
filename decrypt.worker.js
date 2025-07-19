import createModule from './wasm/ncmdump.js';

let wasmModule = null;
let isWasmReady = false;

createModule({
    locateFile: (path) => path.endsWith('.wasm') ? './wasm/ncmdump.wasm' : path
}).then((instance) => {
    wasmModule = instance;
    isWasmReady = true;
    self.postMessage({ type: 'wasm-ready' });
}).catch((err) => {
    self.postMessage({
        type: 'error',
        error: `WASM加载失败: ${err.message}`
    });
});

self.onmessage = async (e) => {
    const { type, payload } = e.data;

    if (type === 'decrypt') {
        try {
            if (!isWasmReady) {
                throw new Error("WASM模块尚未初始化完成");
            }

            const fileData = new Uint8Array(payload.fileData);
            const baseName = payload.baseNameWithoutExtension || "output";

            const inputDataPtr = wasmModule._malloc(fileData.length);
            if (!inputDataPtr) {
                throw new Error("无法为输入文件数据分配WASM内存");
            }

            wasmModule.HEAPU8.set(fileData, inputDataPtr);

            const resultView = wasmModule.decryptNCM(inputDataPtr, fileData.length);

            const result = new Uint8Array(resultView.length);
            result.set(resultView);

            self.postMessage({
                type: 'decrypted',
                payload: {
                    index: payload.index,
                    result: result.buffer
                }
            }, [result.buffer]);

        } catch (error) {
            self.postMessage({
                type: 'error',
                payload: {
                    index: payload.index,
                    error: error.message
                }
            });
        } finally {
            if (inputDataPtr) {
                wasmModule._free(inputDataPtr);
            }
        }
    }
};
