// This code is licensed under the MIT License. Copyright (c) 2020 Soichiro Sugimoto.

#define EXPORT_API __declspec(dllexport)

#include "pch.h"
#include "zdepth.hpp"

extern "C"
{
    EXPORT_API zdepth::DepthCompressor* CreateZdepthCompressor()
    {
        return new zdepth::DepthCompressor();
    }

    EXPORT_API void DeleteZdepthCompressor(zdepth::DepthCompressor* compressor)
    {
        delete compressor;
    }

    EXPORT_API int CompressZdepth(zdepth::DepthCompressor* compressor, int width, int height, const uint16_t *depth, uint8_t* compressedData, bool keyframe)
    {
        std::vector<uint8_t> compressed;
        compressor->Compress(width, height, depth, compressed, keyframe);

        for(auto itr = compressed.begin(); itr != compressed.end(); itr++)
        {
            *compressedData++ = *itr;
        }

        return (int) compressed.size();
    }

    EXPORT_API int DecompressZdepth(zdepth::DepthCompressor* decompressor, int width, int height, int compressedSize, const uint8_t* compressedData, uint16_t* depth)
    {
        std::vector<uint8_t> compressed(compressedData, compressedData + compressedSize);
        std::vector<uint16_t> decompressed;

        zdepth::DepthResult result = decompressor->Decompress(compressed, width, height, decompressed);

        for(auto itr = decompressed.begin(); itr != decompressed.end(); itr++)
        {
            *depth++ = *itr;
        }

        return (int) result;
    }
}
